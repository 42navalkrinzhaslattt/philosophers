/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nam-vu <nam-vu@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 07:25:59 by nam-vu            #+#    #+#             */
/*   Updated: 2024/04/11 07:25:59 by nam-vu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
//#include "../includes/philo.h"

void	init_groups(t_data *data)
{
	int i;

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
		pthread_mutex_init(&data->group[i].count, NULL);
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
		data->philo[i].data = data;
		data->philo[i].index = i + 1;
		data->philo[i].funeral = 0;
		data->philo[i].fork_l = &data->forks[(i - 1 + data->nb_philo) % data->nb_philo];
		data->philo[i].fork_r = &data->forks[(i + data->nb_philo) % data->nb_philo];
		data->philo[i].last_meal = 0;
		data->philo[i].start_time = 0;
		data->philo[i].left_meal = data->nb_eat;
		data->philo[i].last_meal = 0;
		pthread_mutex_init(&data->philo[i].meal, NULL);
		pthread_mutex_init(&data->forks[i], NULL);
		data->philo[i].start_time = ft_gettime(0);
	}
}

int	parse_input(t_data *data, int ac, char **av)
{
	data->forks = NULL;
	data->philo = NULL;
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
	data->forks = malloc(data->nb_philo * sizeof(t_mutex));
	data->philo = malloc(data->nb_philo * sizeof(t_philo));
	if (!data->forks || !data->philo)
		return (print_error(ERR_MALLOC_STATUS, data));
	pthread_mutex_init(&data->write, NULL);
	pthread_mutex_init(&data->death, NULL);
	pthread_mutex_init(&data->flag, NULL);
	init_philo(data);
	return (0);
}
