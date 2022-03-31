/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_abs.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qbrechbu <qbrechbu@student.42lausanne.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/12/02 15:41:43 by qbrechbu          #+#    #+#             */
/*   Updated: 2022/01/10 16:24:43 by qbrechbu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	absolute(int nbr)
{
	if (nbr < 0)
		return (nbr * -1);
	return (nbr);
}