echo "\033pSystem status report\033q\n"
echo "Your system is running TOS version $tos_ver"
echo "Your current NeoDesk version is $neo_ver"
echo "This NeoDesk CLI is version $cli_ver"
if $at_neo
  echo "You are currently at NeoDesk."
else
  echo "NeoDesk is resident, but you are inside another program."
endif
exit