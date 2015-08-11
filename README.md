# surftool
Module to turn squidguard groups(/acls) on or off

Manage your squidguard groups/acls. This tool was made for schools. You can set every group very easily to one of five modes. The modes on/off/only/on plus and adminfree are available. This works also with other "target categories" from shalla or own "target categories".

We made for every classroom a acl. And set for every classroom specific acls (block porn and so on). With this tool you can modify very easily the "Default access [all]" entry. Go to the surftool and click on the bottom of the classroom. This tool is simple to use.

The surf tool consists of two modules.
- The Web GUI reads the squidguard.conf file an writes the user commands to a separate file
- The "surftooldeamon" looks every x seconds for new command files and activates the changes
