#!/bin/bash

# ~/.bashrc skeleton

##### DEFAULTS #####

# If not running interactively, don't do anything
[ -z "$PS1" ] && return

# if the git autocomplete script exists, execute it
[ -f ~/.git-completion.bash ] && . ~/.git-completion.bash
[ -f ~/.git-prompt.bash ] && . ~/.git-prompt.bash

# Custom command prompt
BLACK='\[\e[30m\]'
RED='\[\e[31m\]'
GREEN='\[\e[32m\]'
YELLOW='\[\e[33m\]'
BLUE='\[\e[34m\]'
MAGENTA='\[\e[35m\]'
CYAN='\[\e[36m\]'
LIGHTGRAY='\[\e[37m\]'
DARKGRAY='\[\e[90m\]'
LIGHTRED='\[\e[91m\]'
LIGHTGREEN='\[\e[92m\]'
LIGHTYELLOW='\[\e[93m\]'
LIGHTBLUE='\[\e[94m\]'
LIGHTMAGENTA='\[\e[95m\]'
LIGHTCYAN='\[\e[96m\]'
WHITE='\[\e[97m\]'
ENDCOLOR='\[\e[0m\]'
export PS1="\n$YELLOW\w$LIGHTYELLOW\$(__git_ps1) $ENDCOLOR\n$LIGHTGRAY$ $ENDCOLOR"

export HISTCONTROL=ignoreboth
export EDITOR="vim"
export VISUAL="vim"
export LD_LIBRARY_PATH=""


# ls colors
export LS_COLORS='fi=37:di=32:ln=36:ex=31:*.o=35'

# Prevent Ctrl-d from exiting the shell
set -o ignoreeof

# VI editing mode at command prompt
set -o vi

shopt -s histappend
shopt -s checkwinsize

# Make less more friendly for non-text input files, see lesspipe(1)
[ -x /usr/bin/lesspipe ] && eval "$(SHELL=/bin/sh lesspipe)"

# Color prompt
force_color_prompt=yes

# If this is an xterm set the title to user@host:dir
case "$TERM" in
screen|xterm*|rxvt*)
  PS1="\n$YELLOW\w$LIGHTYELLOW\$(__git_ps1) $ENDCOLOR\n$LIGHTGRAY$ $ENDCOLOR"
    ;;
*)
    ;;
esac

# enable programmable completion features (you don't need to enable
# this, if it's already enabled in /etc/bash.bashrc and /etc/profile
# sources /etc/bash.bashrc).
if [ -f /etc/bash_completion ]; then
    . /etc/bash_completion
fi

##### CUSTOM STARTS HERE #####

## Keeping things organized
#alias ls='ls --color=auto'
alias ll='ls -l'
alias la='ls -A'
alias cp='cp -i'
alias mv='mv -i'
alias df='df -h'
alias du='du -h -c'
alias wget='wget -c'

if [ 'screen' == "${TERM}" ]; then
  export PROMPT_COMMAND='printf "\e]2;%s %s\a" "${USER}" "${PWD}" '
fi

# Export your DYLD or LD libary path variables here
