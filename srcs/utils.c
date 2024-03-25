//#include "../includes/philo.h"
#include "philo.h"

void	exit_error(int status, t_data *data)
{
	write(2, "philo: ", 7);
	if (status == INV_ARG_STATUS)
		write(2, INV_ARG_MSG, 18);
	if (status == ERR_MALLOC_STATUS)
		write(2, ERR_MALLOC_MSG, 13);
	if (status != INV_ARG_STATUS)
		free_all(data);
	exit(status);
}

long	ft_atoi(const char *nptr)
{
	int		i;
	long	res;
	int		sign;

	i = 0;
	res = 0;
	sign = 1;
	while ((nptr[i] >= 9 && nptr[i] <= 13) || nptr[i] == 32)
		i++;
	if (nptr[i] == '-' || nptr[i] == '+')
	{
		if (nptr[i] == '-')
			sign = -1;
		i++;
	}
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		res = res * 10 + nptr[i] - 48;
		if (res * sign > INT32_MAX || res * sign < INT32_MIN)
			return (-2);
		i++;
	}
	if (nptr[i] != 0)
		exit_error(INV_ARG_STATUS, NULL);
	return (res * sign);
}

long long	ft_gettime(long long start)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000 - start);
}

void	free_all(t_data *data)
{
	int i;

	i = -1;
	if (data->nb_philo)
	{
		while (++i < data->nb_philo)
			pthread_mutex_destroy(&data->forks[i]);
		free(data->philo);
	}
	pthread_mutex_destroy(&data->write);
	pthread_mutex_destroy(&data->death);
	if (data->forks)
		free(data->forks);
}
