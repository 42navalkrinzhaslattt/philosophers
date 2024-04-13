/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nam-vu <nam-vu@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 07:25:59 by nam-vu            #+#    #+#             */
/*   Updated: 2024/04/11 07:25:59 by nam-vu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"
#include "philo.h"

void	beholder(t_philo *philo)
{
	int	i;

	while (1)
	{
		usleep(500);
		pthread_mutex_lock(&philo->meal);
		if (philo->left_meal && ft_gettime(philo->start_time + philo->last_meal)
			> philo->data->die_ms)
		{
			action_log(philo, DIE, -1);
			pthread_mutex_lock(&philo->data->death);
			i = -1;
			while (++i < philo->data->nb_philo)
				philo->data->philo[i].funeral = 1;
			pthread_mutex_unlock(&philo->data->death);
			pthread_mutex_unlock(&philo->meal);
			return ;
		}
		else if (!philo->left_meal)
			return (pthread_mutex_unlock(&philo->meal), free(NULL));
		pthread_mutex_unlock(&philo->meal);
	}
}

int	main(int ac, char **av)
{
	t_data	data;
	int		i;
	t_pth	control;

	if (parse_input(&data, ac, av))
		return (EXIT_FAILURE);
	init_groups(&data);
	i = -1;
	while (++i < data.nb_philo)
		pthread_create(&data.philo[i].pthread,
			NULL, (void *)routine, &data.philo[i]);
	pthread_create(&control, NULL, (void *)controler, &data);
	i = -1;
	while (++i < data.nb_philo)
		pthread_create(&data.philo[i].monitor,
			NULL, (void *)beholder, &data.philo[i]);
	i = -1;
	while (++i < data.nb_philo)
		pthread_join(data.philo[i].pthread, NULL);
	i = -1;
	while (++i < data.nb_philo)
		pthread_join(data.philo[i].monitor, NULL);
	pthread_join(control, NULL);
	free_all(&data);
	return (EXIT_SUCCESS);
}
