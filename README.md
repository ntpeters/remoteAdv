The purpose of this project is to provide an easy means of remotely connecting
to an unsuspecting "infected" user's computer and executing various commands in
the background.  These commands will primarily be for the purpose of trolling
infected individuals for our amusement, and will be mostly non-destructive in
nature.

All clients will connect to a remote server that will act as a hub for all
client communications.

There will be three different client "levels":

    Slave Clients   - These clients are installed on target user's computers.
                      They are unable to do anything without receiving a command
                      from another client via the server.

    Manager Clients - These clients are able to connect to and control slaves
                      by issueing any command that managers are permitted to
                      execute.

    Master Clients  - These clients have full and unbridled access to both
                      slaves and managers.  Masters are able to execute
                      privileged commands on slaves, and are able to treat
                      managers as slaves.