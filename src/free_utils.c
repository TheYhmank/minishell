/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayermeko <ayermeko@student.42prague.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/20 20:15:39 by ayermeko          #+#    #+#             */
/*   Updated: 2024/09/25 11:44:17 by ayermeko         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

char	*free_spaces(char *input)
{
	char	*result;

	result = trim_spaces_no_free(input);
	free(input);
	return (result);
}

void	free_array(char **av)
{
	int	i;

	if (!av)
		return ;
	i = 0;
	while (av[i])
	{
		free(av[i]);
		av[i] = NULL;
		i++;
	}
	free(av);
	av = NULL;
}
