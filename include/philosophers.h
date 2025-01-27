/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akostian <akostian@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 19:42:50 by akostian          #+#    #+#             */
/*   Updated: 2025/01/27 11:59:02 by akostian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

#include "ANSI_colors.h"

#define INCORRECT_ARGS_ERROR_MESSAGE \
"\033[0;31mIncorrect arguments\033[0m\n\
Usage: ./philo \
<number_of_philosophers> \
<time_to_die> \
<time_to_eat> \
<time_to_sleep> \
<number_of_times_each_philosopher_must_eat>\n"

typedef struct s_fork
{
	unsigned int	fork_n;
	unsigned int	is_taken;
	pthread_mutex_t	mutex;
}	t_fork;

typedef struct s_simulation_settings
{
	unsigned int		philo_n;		//number_of_philosophers
	time_t				time_to_die;
	time_t				time_to_eat;
	time_t				time_to_slp;
	unsigned int		ph_eat_count;	//nbr of times each philosopher must eat
}	t_sim_settings;

typedef struct s_ph_thread_args
{
	t_sim_settings		*sim;			// Simulation settings
	time_t				start_time;		// Start time of the program
	unsigned short int	philo_n;
	pthread_mutex_t		*print_mutex;	// Mutex for printf
	t_fork				*left_fork;
	t_fork				*right_fork;
}	t_ph_thread_args;

int				ft_atoi(const char *str);
int				is_all_digit(char *str);
unsigned char	parse(int argc, char **argv, t_sim_settings *sim_settings);

int				ft_imax(int a, int b);

time_t			get_time_ms(void);
time_t			get_time_us(void);
