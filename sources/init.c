/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vvermot- <marvin@42lausanne.ch>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/03 16:02:57 by vvermot-          #+#    #+#             */
/*   Updated: 2022/05/03 16:02:59 by vvermot-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/cub3d.h"

void	init_cardi_struct(t_cardi_check *cardi)
{
	cardi->val = malloc(sizeof(int) * 4);
	cardi->val[0] = 0;
	cardi->val[1] = 0;
	cardi->val[2] = 0;
	cardi->val[3] = 0;
	cardi->cardi = malloc(sizeof(char *) * 4);
	cardi->cardi[0] = ft_strcpy("NO");
	cardi->cardi[1] = ft_strcpy("SO");
	cardi->cardi[2] = ft_strcpy("WE");
	cardi->cardi[3] = ft_strcpy("EA");
}

void	init_config_struct(t_config *cfg)
{
	cfg->path_no = NULL;
	cfg->path_ea = NULL;
	cfg->path_so = NULL;
	cfg->path_we = NULL;
}
