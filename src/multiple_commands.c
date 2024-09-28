/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multiple_commands.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayermeko <ayermeko@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/28 10:30:45 by ayermeko          #+#    #+#             */
/*   Updated: 2024/09/28 11:04:29 by ayermeko         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

static void	clean_after_execute(int *children_pid)
{
	close_extra_fds();
	free(children_pid);
}

static int	wait_for_children(int children_pid[1024])
{
	int	i;
	int	exit_status;
	int	is_last_child;

	i = 0;
	exit_status = 0;
	while (children_pid[i] != 0)
	{
		is_last_child = children_pid[i + 1] == 0;
		exit_status = wait_for_child(children_pid[i], is_last_child);
		i++;
	}
	clean_after_execute(children_pid);
	return (exit_status);
}

void	handle_pipe(int original_fd_out, char *curr_cmd, char **commands)
{
	int			is_first_command;
	int			has_next_command;
	char		*last_command;
	static int	pipe_fds[2];

	last_command = commands[arr_len(commands) - 1];
	is_first_command = (curr_cmd == commands[0]);
	has_next_command = (curr_cmd != last_command);
	if (!is_first_command)
		redirect_fd(pipe_fds[0], 0);
	if (has_next_command)
	{
		if (pipe(pipe_fds) == -1)
			print_perror_msg("pipe", curr_cmd);
		redirect_fd(pipe_fds[1], 1);
	}
	else
		redirect_fd(original_fd_out, 1);
}

int	arr_len(char **arr)
{
	int	len;

	len = 0;
	while (*arr)
	{
		len++;
		arr++;
	}
	return (len);
}

int	*init_children_pid(char **commands)
{
	int		*children_pid;
	int		size;

	size = sizeof(int) * (arr_len(commands) + 1);
	children_pid = malloc(size);
	if (!children_pid)
		return (NULL);
	ft_bzero(children_pid, size);
	return (children_pid);
}

static void	save_original_fds(int original_fds[2])
{
	original_fds[0] = dup(0);
	original_fds[1] = dup(1);
}

int	multiple_commands(char **commands, t_env **minienv)
{
	int	original_fds[2];
	int	*children_pid;
	int	i;

	save_original_fds(original_fds);
	children_pid = init_children_pid(commands);
	i = 0;
	while (commands[i])
	{
		handle_pipe(original_fds[1], commands[i], commands);
		children_pid[i] = fork();
		define_execute_signals(children_pid[i]);
		if (children_pid[i] == -1)
			print_perror_msg("fork", commands[i]);
		else if (children_pid[i] == 0)
		{
			free(children_pid);
			handle_child_redir(commands[i], commands, minienv);
			execute_fork_cmd(commands[i], commands, minienv); // UNDONE
		}
		i++;
	}
	restore_original_fds(original_fds);
	return (wait_for_children(children_pid));
}