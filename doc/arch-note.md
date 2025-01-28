
### **1. Indirect References (Handle/ID System)**
#### **Concept**
- Replace raw pointers with **immutable unique identifiers** (e.g., client file descriptors, session IDs).
- Channels store these IDs instead of pointers, decoupling them from `Client` object lifetimes.

#### **Mechanics**
- **Assign IDs**: Use the client’s file descriptor (e.g., `int fd`) as the ID, since it’s unique and already tracked in your `ServerData`’s `clients` map.
- **Store IDs in Channels**:
  ```cpp
  class Channel {
  private:
      std::vector<int> memberIDs; // Stores client file descriptors (IDs)
  };
  ```
- **Access Clients via IDs**:
  ```cpp
  // Example: Send a message to all members in a channel
  void Channel::broadcast(ServerData& servData, const std::string& msg) {
      for (std::vector<int>::iterator it = memberIDs.begin(); it != memberIDs.end();) {
          std::map<int, Client>::iterator clientIt = servData.clients.find(*it);
          if (clientIt != servData.clients.end()) {
              clientIt->second.send(msg);
              ++it;
          } else {
              // Remove invalid ID
              it = memberIDs.erase(it);
          }
      }
  }
  ```

#### **Pros**
- No dangling pointers (channels don’t own `Client` objects).
- Decouples `Channel` from `Client` lifetimes.
- Easy to serialize/deserialize (IDs are integers).

---

### **2. Central Registry (ServerData)**
#### **Concept**
- Use `ServerData` as the **single source of truth** for validating client existence.
- All client lookups go through `ServerData::clients`.

#### **Mechanics**
- **Registry Structure**:
  ```cpp
  struct ServerData {
      std::map<int, Client> clients; // Key = client file descriptor (ID)
      std::vector<Channel> channels;
  };
  ```
- **Validation Workflow**:
  1. When a client disconnects, remove it from `ServerData::clients`.
  2. Channels check if an ID exists in `ServerData::clients` before accessing the `Client`.

#### **Example: Client Disconnection**
```cpp
void ServerData::removeClient(int clientID) {
    std::map<int, Client>::iterator it = clients.find(clientID);
    if (it != clients.end()) {
        clients.erase(it);
    }
}
```

#### **Pros**
- Centralized client management.
- No scattered cleanup logic.

---

### **3. Periodic Stale ID Cleanup**
#### **Concept**
- Channels periodically purge IDs that no longer map to valid clients in `ServerData`.

#### **Mechanics**
- **Cleanup Trigger**:
  - Run during server ticks (e.g., after processing client input).
  - Example cleanup loop:
    ```cpp
    void ServerData::cleanupStaleIDs() {
        for (std::vector<Channel>::iterator ch = channels.begin(); ch != channels.end(); ++ch) {
            std::vector<int>& memberIDs = ch->getMemberIDs();
            for (std::vector<int>::iterator it = memberIDs.begin(); it != memberIDs.end();) {
                if (clients.find(*it) == clients.end()) {
                    it = memberIDs.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }
    ```
- **Optimization**:
  - Cleanup can be batched (e.g., every 10 server ticks).
  - Alternatively, trigger cleanup when a client is explicitly removed.

#### **Pros**
- Minimal runtime overhead.
- Guarantees channels only reference valid clients.

---

### **Workflow Summary**
1. **Client Joins**:
   - Add client to `ServerData::clients` with its `fd` as the key.
   - Channels store the client’s `fd` in their `memberIDs`.

2. **Client Leaves**:
   - Remove client from `ServerData::clients`.
   - Stale `fd` in channel `memberIDs` are purged during the next cleanup cycle.

3. **Channel Operations**:
   - Always validate client IDs against `ServerData::clients` before use.

---

### **Example Scenario**
- **Client A** (`fd=4`) joins **Channel #general**.
  - `ServerData::clients` adds entry `4 → Client A`.
  - `Channel #general` adds `4` to `memberIDs`.
- **Client A** disconnects.
  - `ServerData::clients` removes entry `4`.
- During the next server tick:
  - `cleanupStaleIDs()` iterates through `Channel #general`’s `memberIDs`.
  - `4` is not found in `ServerData::clients`, so it’s removed from `memberIDs`.

---

### **Pros/Cons Tradeoffs**
| **Pros**                                      | **Cons**                                  |
|-----------------------------------------------|-------------------------------------------|
| No dangling pointers                          | Slight overhead for ID validation         |
| Decoupled architecture                        | Stale IDs exist until cleanup             |
| Works with C++98                              | Manual cleanup logic                      |
| Easy to debug (IDs are simple integers)       | Channels don’t "know" when clients leave  |

