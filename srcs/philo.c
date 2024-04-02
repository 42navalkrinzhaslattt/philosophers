#include "../includes/philo.h"
#include "philo.h"

//start
//think
//take a left fork if free - if not wait
//take a right fork if free - if not wait
//eat
//sleep
//repeat if nobody died and philo isnt finished
//when somebody is writing, nobody should die or write

void	action_log(t_philo *philo, int action)
{
	pthread_mutex_lock(&philo->data->death);
	pthread_mutex_lock(&philo->data->write);
	if (!philo->funeral)
		printf("%lli %d ", ft_gettime(philo->start_time), philo->index);
	if (action == FORK && !philo->funeral)
		printf("has taken a fork\n");
	else if (action == EAT && !philo->funeral)
		printf("is eating\n");
	else if (action == SLEEP && !philo->funeral)
		printf("is sleeping\n");
	else if (action == THINK && !philo->funeral)
		printf("is thinking\n");
	else if (action == DIE && !philo->funeral)
		printf("died\n");
	pthread_mutex_unlock(&philo->data->write);
	pthread_mutex_unlock(&philo->data->death);
}

void	eat_even(t_philo *philo)
{
	if (ft_gettime(philo->last_meal)  < philo->data->eat_ms + philo->data->sleep_ms + 10)
		usleep(500);
	pthread_mutex_lock(philo->fork_r);
	action_log(philo, FORK);
	pthread_mutex_lock(philo->fork_l);
	action_log(philo, FORK);
	pthread_mutex_lock(&philo->meal);
	if (philo->left_meal)
		philo->left_meal--;
	philo->last_meal = ft_gettime(philo->start_time);
	action_log(philo, EAT);
	pthread_mutex_unlock(&philo->meal);
	usleep(philo->data->eat_ms * 1000);
	pthread_mutex_unlock(philo->fork_l);
	pthread_mutex_unlock(philo->fork_r);
}

void	eat_odd(t_philo *philo)
{
	if (ft_gettime(philo->last_meal)  < philo->data->eat_ms + philo->data->sleep_ms + 10)
		usleep(500);//???
	pthread_mutex_lock(philo->fork_l);
	action_log(philo, FORK);
	pthread_mutex_lock(philo->fork_r);
	action_log(philo, FORK);
	pthread_mutex_lock(&philo->meal);
	if (philo->left_meal)
		philo->left_meal--;
	philo->last_meal = ft_gettime(philo->start_time);
	action_log(philo, EAT);
	pthread_mutex_unlock(&philo->meal);
	usleep(philo->data->eat_ms * 1000);
	pthread_mutex_unlock(philo->fork_r);
	pthread_mutex_unlock(philo->fork_l);
}

void	beholder(t_philo *philo)
{
	int i;

	while (1)
	{
		pthread_mutex_lock(&philo->meal);
		if (philo->left_meal && ft_gettime(philo->start_time + philo->last_meal) > philo->data->die_ms)
		{
			action_log(philo, DIE);
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

void	routine(t_philo *philo)
{
	pthread_mutex_lock(&philo->data->death);
	while (philo->left_meal && !philo->funeral)
	{
		pthread_mutex_unlock(&philo->data->death);
		action_log(philo, THINK);
		if (philo->data->nb_philo == 1)
			return ;
		if (philo->index % 2)
			eat_odd(philo);
		else
			eat_even(philo);
		action_log(philo, SLEEP);
		usleep(philo->data->sleep_ms * 1000);
		pthread_mutex_lock(&philo->data->death);
	}
	pthread_mutex_unlock(&philo->data->death);
}

int	main(int ac, char **av)
{
	t_data	data;
	int		i;

	if (ac != 5 && ac!= 6)
		exit_error(INV_ARG_STATUS, NULL);
	parse_input(&data, ac, av);
	i = -1;
	while (++i < data.nb_philo)
		pthread_create(&data.philo[i].pthread, NULL, (void *)routine, &data.philo[i]);
	i = -1;
	while (++i < data.nb_philo)
		pthread_create(&data.philo[i].monitor, NULL, (void *)beholder, &data.philo[i]);
	i = -1;
	while (++i < data.nb_philo)
		pthread_join(data.philo[i].pthread, NULL);
	i = -1;
	while (++i < data.nb_philo)
		pthread_join(data.philo[i].monitor, NULL);
	free_all(&data);
	return (EXIT_SUCCESS);
}
