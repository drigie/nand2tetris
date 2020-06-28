_HardwareSimulator()
{
  local cur=${COMP_WORDS[COMP_CWORD]}

  local IFS=$'\n'
  COMPREPLY=( $( compgen -f -X '!*.tst' -- $cur ) )
}

complete -o filenames -F _HardwareSimulator HardwareSimulator.sh