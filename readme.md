### How to use in a Docker container

1. Build the image:
   `docker build --no-cache -t frmudlib .`
2. Run the container in background:
   `docker run --rm -td -p 23:5000 frmudlib`
3. Or run the container in interactive mode:
   `docker run --rm -ti -p 23:5000 frmudlib`
   
We are mapping the 23 port in the host machine to the 5000 port in the container, so if we execute `telnet localhost` in the host machine it will connect to the mud in the container.

### Connecting to the game

- Use in your local machine `telnet localhost` (it will use the default port 23) to connect to the mud inside the container and see if everything is working.

By default you will have an administrator account created, with username `god` and password `god`.