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

# include <semaphore.h>
# include <sys/types.h>
# include <sys/time.h>
# include <sys/wait.h>
# include <string.h>
# include <pthread.h>
# include <signal.h>
# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include <fcntl.h>
# include <limits.h>
# include <stdint.h>

# define THINK 1
# define SLEEP 2
# define EAT 3
# define FORK 4
# define DIE 5

# define SEM_FORKS "/fork"
# define SEM_WRITE "/write"
# define SEM_DEATH "/death"
# define SEM_FLAG "/flag"

# define INV_ARG_STATUS 1
# define ERR_MALLOC_STATUS 2

# define INV_ARG_MSG "Invalid arguments\n"
# define ERR_MALLOC_MSG "Malloc error\n"

typedef pthread_t		t_pth;
typedef sem_t			t_sem;
typedef pid_t			t_pid;
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
	char		count_name[4];
	sem_t 		*count;
}	t_group;

typedef struct s_philo
{
	sem_t			*forks;
	sem_t			*meal;
	char			meal_name[6];
	t_pid			pid;
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
	sem_t		*forks;
	sem_t		*write;
	sem_t		*death;
	sem_t		*flag;
	t_pid		control;
	t_pid		maniac;
}	t_data;

//philo.c
void	ft_usleep(long long amount);
void	beholder(t_philo *philo);
int		create_child(t_data *data, int i);
int		kill_childs(t_data *data);
int		main(int ac, char **av);

//parse.c
void	init_group_semaphore(t_data *data, int index);
void	init_groups(t_data *data);
void	init_philo(t_data *data);
int		parse_input(t_data *data, int ac, char **av);

//routine.c
int		controler(t_data *data);
void	philo_eat(t_philo *philo, int eat_start);
void	philo_round(t_philo *philo, t_data *data,
					int eat_start, int sleep_start);
void	routine(t_philo *philo);

//utils.c
void	free_all(t_data *data);
void	action_log(t_philo *philo, int action, long long timestamp);
int	print_error(int status, t_data *data);
long	ft_atoi(const char *nptr);
long long	ft_gettime(long long start);

#endif