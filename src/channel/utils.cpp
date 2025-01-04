/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlamkadm <mlamkadm@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/04 16:12:27 by mlamkadm          #+#    #+#             */
/*   Updated: 2025/01/04 16:12:27 by mlamkadm         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../inc/Channel.hpp"

// Utils and Channel helpers


void addChannel(std::vector<Channel>& channels, const std::string& channelName) {
    channels.push_back(Channel(channelName));
}

