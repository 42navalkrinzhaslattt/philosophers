/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nam-vu <nam-vu@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/13 00:30:48 by nam-vu            #+#    #+#             */
/*   Updated: 2024/04/13 00:30:48 by nam-vu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <stdint.h>
# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <pthread.h>
# include <sys/time.h>

# define THINK 1
# define SLEEP 2
# define EAT 3
# define FORK 4
# define DIE 5

# define INV_ARG_STATUS 1
# define ERR_MALLOC_STATUS 2

# define INV_ARG_MSG "Invalid arguments\n"
# define ERR_MALLOC_MSG "Malloc error\n"

typedef pthread_mutex_t	t_mutex;
typedef pthread_t		t_pth;
typedef struct s_data	t_data;
typedef struct s_group	t_group;
typedef struct s_philo	t_philo;

typedef struct s_group
{
	int			index;
	int			round_len;
	int			eat_ms;
	int			size;
	int			counter;
	t_mutex		count;
}	t_group;

typedef struct s_philo
{
	t_mutex			*fork_l;
	t_mutex			*fork_r;
	t_mutex			meal;
	t_pth			pthread;
	t_pth			monitor;
	int				index;
	int				left_meal;
	int				funeral;
	long long		last_meal;
	long long		start_time;
	t_group			*group;
	t_data			*data;
}	t_philo;

typedef struct s_data
{
	int			nb_philo;
	int			die_ms;
	int			eat_ms;
	int			sleep_ms;
	int			nb_eat;
	int			nb_group;
	int			eat_flag;
	t_philo		*philo;
	t_group		*group;
	t_mutex		*forks;
	t_mutex		write;
	t_mutex		death;
	t_mutex		flag;
}	t_data;

//philo.c
void		ft_usleep(long long amount);
void		beholder(t_philo *philo);
int			main(int ac, char **av);

//parse.c
void		init_groups(t_data *data);
void		init_philo(t_data *data);
int			parse_input(t_data *data, int ac, char **av);

//utils.c
void		action_log(t_philo *philo, int action, long long timestamp);
int			print_error(int status, t_data *data);
long		ft_atoi(const char *nptr);
long long	ft_gettime(long long start);
void		free_all(t_data *data);

//routine.c
void		philo_eat(t_philo *philo, int eat_start);
void		philo_round(t_philo *philo, t_data *data,
				int eat_start, int sleep_start);
void		routine(t_philo *philo);
int			check_spinlock(t_data *data);
void		controler(t_data *data);

#endif