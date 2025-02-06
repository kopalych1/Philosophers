/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akostian <akostian@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 19:42:50 by akostian          #+#    #+#             */
/*   Updated: 2025/02/07 00:21:18 by akostian         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILOSOPHERS_H
# define PHILOSOPHERS_H
# include <pthread.h>
# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <sys/time.h>
# include <string.h>
# include <error.h>
# include <errno.h>

# ifndef ENOMEM
#  define ENOMEM 12
# endif

# include "ANSI_colors.h"

# ifndef COLORED
#  define COLORED 0
# endif

# if COLORED
#  define PHILO_N "\033[1;33m%-4d\033[0m"
#  define PHILO_EATING_MSG "\033[1;31mis eating\n\033[0m"
#  define PHILO_SLEEPING_MSG "\033[1;34mis sleeping\n\033[0m"
#  define PHILO_THINKING_MSG "\033[1;32mis thinking\n\033[0m"
#  define PHILO_TAKING_MSG "\033[1;35mhas taken a fork\n\033[0m"
#  define PHILO_DIED_MSG  "\033[1;30mdied\n\033[0m"
# else
#  define PHILO_N "%-4d"
#  define PHILO_EATING_MSG "is eating\n"
#  define PHILO_SLEEPING_MSG "is sleeping\n"
#  define PHILO_THINKING_MSG "is thinking\n"
#  define PHILO_TAKING_MSG "has taken a fork\n"
#  define PHILO_DIED_MSG "died\n"
# endif

# define INCORRECT_ARGS_ERROR_MESSAGE \
"\033[41mIncorrect arguments\033[0m\n\
Usage: ./philo \
<number_of_philosophers> \
<time_to_die> \
<time_to_eat> \
<time_to_sleep> \
<number_of_times_each_philosopher_must_eat>\n"

# define NO_MEMORY_ERROR_MESSAGE \
"\033[41mNot enough memomry\033[0m\n"

typedef struct s_simulation_settings
{
	unsigned int		philo_n;		//number_of_philosophers
	time_t				time_to_die;
	time_t				time_to_eat;
	time_t				time_to_slp;
	unsigned int		ph_eat_count;	//nbr of times each philosopher must eat
	pthread_mutex_t		*forks;
	pthread_mutex_t		*print_mutex;
	unsigned char		is_dead;
	struct s_ph_data	*philo_data;
	time_t				*last_meals;
	unsigned char		*eat_enough;
}	t_sim_settings;

typedef struct s_ph_data
{
	t_sim_settings		*sim;			// Simulation settings
	time_t				start_time;		// Start time of the program
	unsigned short int	philo_n;
	pthread_mutex_t		*print_mutex;	// Mutex for printf
	pthread_mutex_t		*left_fork;
	pthread_mutex_t		*right_fork;
}	t_ph_data;

int				ft_atoi(const char *str);
int				is_all_digit(char *str);
unsigned char	parse(int argc, char **argv, t_sim_settings *sim_settings);

int				ft_imax(int a, int b);
void			*ft_calloc(size_t count, size_t size);

time_t			get_time_ms(void);
time_t			get_time_us(void);

#endif