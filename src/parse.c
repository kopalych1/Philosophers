/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akostian <akostian@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 09:43:03 by akostian          #+#    #+#             */
/*   Updated: 2025/01/29 17:30:45 by akostian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philosophers.h"

unsigned char	parse(int argc, char **argv, t_sim_settings *sim_settings)
{
	int	i;

	if (argc != 5 && argc != 6)
		return (1);
	i = 1;
	while (i < argc)
		if (!is_all_digit(argv[i++]))
			return (1);
	sim_settings->philo_n = ft_atoi(argv[1]);
	sim_settings->time_to_die = ft_atoi(argv[2]);
	sim_settings->time_to_eat = ft_atoi(argv[3]);
	sim_settings->time_to_slp = ft_atoi(argv[4]);
	if (argc == 6)
		sim_settings->ph_eat_count = ft_atoi(argv[5]);
	else
		sim_settings->ph_eat_count = 0;
	if (sim_settings->philo_n < 1 || sim_settings->time_to_die < 1
		|| sim_settings->time_to_eat < 1 || sim_settings->time_to_slp < 1)
		return (1);
	return (0);
}
