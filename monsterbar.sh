#!/bin/sh

fifo="/tmp/m"

trap 'rm -f "${fifo}"' INT TERM EXIT
[ -p "${fifo}" ] || { rm -f "${fifo}"; mkfifo -m 600 "${fifo}"; }

"${HOME}"/projects/mstatusbar/mstatusbar < "${fifo}" | bar &

"${HOME}"/projects/monsterwm/monsterwm > "${fifo}"
