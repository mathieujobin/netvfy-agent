-- Run in background so that following commands can execute while dnc is running. Redirection to /dev/null is necessary for background.
do shell script "/Applications/netvirt-agent.app/Contents/MacOS/netvirt-agent > /dev/null 2>&1 &" with administrator privileges
