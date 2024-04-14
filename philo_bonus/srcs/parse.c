/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nam-vu <nam-vu@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/14 10:14:17 by nam-vu            #+#    #+#             */
/*   Updated: 2024/04/14 10:14:17 by nam-vu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	init_group_semaphore(t_data *data, int index)
{
	data->group[index].count_name[0] =  '/';
	data->group[index].count_name[1] =  'G';
	data->group[index].count_name[2] =  index + 48;
	data->group[index].count_name[3] =  0;
	sem_unlink(data->group[index].count_name);
	data->group[index].count = sem_open(data->group[index].count_name, O_CREAT | O_EXCL, 0700, 0);
}

void	init_groups(t_data *data)
{
	int	i;

	data->nb_group = 2 + (data->nb_philo % 2 == 1);
	data->group = malloc(sizeof(t_group) * data->nb_group);
	i = -1;
	while (++i < data->nb_group)
	{
		data->group[i].index = i;
		data->group[i].eat_ms = data->eat_ms;
		data->group[i].round_len = data->sleep_ms;
		if ((data->nb_group - 1) * data->eat_ms > data->sleep_ms)
			data->group[i].round_len = (data->nb_group - 1) * data->eat_ms;
		data->group[i].round_len += data->eat_ms;
		data->group[i].size = 1 * (!i && data->nb_group == 3)
			+ (data->nb_philo / 2) * (i || data->nb_group == 2);
		data->group[i].counter = 0;
		if (i == data->nb_group - 1)
			data->group[i].counter = data->group[i].size;
		init_group_semaphore(data, i);
	}
	i = -1;
	while (++i < data->nb_philo)
		data->philo[i].group = &data->group[(i % 2 == 1)
			+ (data->nb_group == 3) * (2 * (i != 0) * (i % 2 == 0))];
}

void	init_philo(t_data *data)
{
	int	i;

	i = -1;
	while (++i < data->nb_philo)
	{
		data->philo[i].meal_name[0] = '/';
		data->philo[i].meal_name[1] = 'M';
		data->philo[i].meal_name[2] = i / 100 + 48;
		data->philo[i].meal_name[3] = i % 100 / 10 + 48;
		data->philo[i].meal_name[4] = i % 10;
		data->philo[i].meal_name[5] = 0;
		sem_unlink(data->philo[i].meal_name);
		data->philo[i].meal = sem_open(data->philo[i].meal_name,
			O_CREAT | O_EXCL, 0700, 1);
		data->philo[i].data = data;
		data->philo[i].index = i + 1;
		data->philo[i].funeral = 0;
		data->philo[i].last_meal = 0;
		data->philo[i].start_time = 0;
		data->philo[i].left_meal = data->nb_eat;
		data->philo[i].last_meal = 0;
		data->philo[i].start_time = ft_gettime(0);
	}
}

int	parse_input(t_data *data, int ac, char **av)
{
	if (ac != 5 && ac != 6)
		return (print_error(INV_ARG_STATUS, NULL));
	data->philo = NULL;
	data->group = NULL;
	data->nb_philo = ft_atoi(av[1]);
	data->die_ms = ft_atoi(av[2]);
	data->eat_ms = ft_atoi(av[3]);
	data->sleep_ms = ft_atoi(av[4]);
	data->nb_eat = -1;
	data->eat_flag = 0;
	if (ac == 6)
		data->nb_eat = ft_atoi(av[5]);
	if (data->nb_philo < 0 || data->die_ms < 0 || data->eat_ms < 0
		|| data->sleep_ms < 0 || data->nb_eat < -1)
		return (print_error(INV_ARG_STATUS, data));
	data->philo = malloc(data->nb_philo * sizeof(t_philo));
	sem_unlink(SEM_FORKS);
	sem_unlink(SEM_WRITE);
	sem_unlink(SEM_DEATH);
	data->forks = sem_open(SEM_FORKS, O_CREAT | O_EXCL, 0700, data->nb_philo);
	data->write = sem_open(SEM_WRITE, O_CREAT | O_EXCL, 0700, 1);
	data->death = sem_open(SEM_DEATH, O_CREAT | O_EXCL, 0700, 0);
	init_philo(data);
	init_groups(data);
	return (0);
}