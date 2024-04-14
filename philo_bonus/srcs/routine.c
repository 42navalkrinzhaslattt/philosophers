/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nam-vu <nam-vu@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/14 10:14:17 by nam-vu            #+#    #+#             */
/*   Updated: 2024/04/14 10:14:17 by nam-vu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	controler(t_data *data)
{
	int	i;
	int	eat_flag;
	int	meals;

	data->control = fork();
	if (data->control == 0)
	{
		if (data->nb_philo == 1)
			return (1);
		meals = 0;
		eat_flag = 0;
		while (data->nb_eat < 0 || (data->nb_eat > 0
			&& meals < 3 * data->nb_eat))
		{
			i = -1;
			while (++i < data->group[eat_flag].size)
				sem_post(data->group[eat_flag].count);
			ft_usleep(data->eat_ms * 1000);
			i = -1;
			while (++i < data->group[eat_flag].size)
				sem_wait(data->group[eat_flag].count);
			eat_flag = (eat_flag + 1) % data->nb_group;
			meals++;
		}
		sem_post(data->death);
		return (1);
	}
	return (0);
}

void	philo_eat(t_philo *philo, int eat_start)
{
	sem_wait(philo->data->forks);
	action_log(philo, FORK, eat_start);
	sem_wait(philo->data->forks);
	action_log(philo, FORK, eat_start);
	action_log(philo, EAT, eat_start);
	sem_wait(philo->meal);
	if (philo->left_meal > 0)
		philo->left_meal--;
	philo->last_meal = ft_gettime(philo->start_time);
	sem_post(philo->meal);
	ft_usleep(philo->group->eat_ms * 1000);
	sem_post(philo->data->forks);
	sem_post(philo->data->forks);
}

void	philo_round(t_philo *philo, t_data *data,
			int eat_start, int sleep_start)
{
	sem_wait(philo->group->count);
	philo_eat(philo, eat_start);
	sem_post(philo->group->count);
	if (!philo->left_meal)
		return ;
	action_log(philo, SLEEP, sleep_start);
	ft_usleep(data->sleep_ms * 1000);
}

void	routine(t_philo *philo)
{
	long long	think_start;
	long long	eat_start;
	long long	sleep_start;

	eat_start = philo->group->index * philo->group->eat_ms;
	think_start = 0;
	sleep_start = eat_start + philo->group->eat_ms;
	action_log(philo, THINK, 0);
	if (philo->data->nb_philo == 1)
	{
		ft_usleep(philo->data->die_ms * 1000);
		action_log(philo, DIE, philo->data->die_ms);
		sem_post(philo->data->death);
		return ;
	}
	while (philo->left_meal)
	{
		ft_usleep((eat_start - think_start) * 1000);
		philo_round(philo, philo->data, eat_start, sleep_start);
		eat_start += philo->group->round_len;
		think_start = eat_start + philo->group->eat_ms
			+ philo->data->sleep_ms - philo->group->round_len;
		sleep_start += philo->group->round_len;
		action_log(philo, THINK, think_start);
	}
}
