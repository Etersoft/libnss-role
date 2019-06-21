# libnss-role

NSS API library and admin tools for roles and privileges.
This README is also available in [Russian (Русский)](README-ru.md)

## What is this?

`libnss_role` is an [NSS](https://en.wikipedia.org/wiki/Name_Service_Switch) module adds support for adding groups to groups.
This makes certain system administration tasks easier.

For example, you can create a group named `users` and add it to groups such as `audio`, `cdwriter`, `serial` etc.
Now you can add all users to `users` group instead of adding them to all these groups manually.

Furthermore, you can create a group named `admins` and add it to groups `users` and `wheel`. As a result, administrators
will get all "user" groups and a `wheel` group in addition.

This module has its own administration utilities. These utilities divide all groups into two categories: **roles** and **privileges**.

### Privilege
This is an ordinary POSIX group that can be assigned to a user.
Members of a "privilege" group can perform certain actions.
Examples of privileges are: `cdwriter`, `audio`, `serial`, `virtualbox` and many others.

### Role
A role is also a POSIX group, but it has another meaning: it describes a type of activity that a user performs.
Examples of roles are: `admin`, `user`, `power_user`, `developer` and others.

A user may need some special rights to perform his role. It is useful to have a possibility of adding sets of privileges to
users indirectly via assigning a role. For example, an user with `admin` role could also get privilege groups such as
`wheel`, `ssh`, `root` etc.

This module implements such permission management model.

## How to build
In order to build this project, you will need:
* scons
* C++ compiler (e.g. gcc-c++)
* headers for glibc, pam

After installing all of the above, enter the directory with the source code and run:
```
$ scons
```

To install, run as superuser:
```
$ scons install
```

Maybe you will need to create a configuration file with role info:
```
$ touch /etc/role
```

Now you need to enable the module. Open `/etc/nsswitch.conf` and append `role` to the end of the line that starts with `groups: ...`.
You should get something like this:
```
groups: files ldap role
```

## Administration

### Configuration file
This module uses `/etc/role` file and a system file `/etc/group` to store role information.

`/etc/group` is a standard POSIX file and is described in many guides.
`/etc/role` stores additional information about groups that are included in other groups.
Format of `/etc/role` is as follows:
```
<group_id>:<group_id>[,<group_id>]*
```
where `<group_id>` is an integer group identifier.

An identifier before `:` means that a group is a role and will be included in other groups.
Identifiers after `:` are identifiers of groups that include this role.
Nested groups are resolved recursively.

Here is an example. Suppose that we have a user named `pupkin` and we have some records in `/etc/group`:
```
group1:x:1:pupkin
group2:x:2:pupkin
group3:x:3:
group4:x:4:
group5:x:5:
group6:x:6:
```

Meanwhile the `/etc/role` file contains:
```
2:3,4
4:5,6
```

With such configuration `pupkin` will get all the groups.
* he gets `group1` and `group2` as they are assigned to him directly;
* he gets `group3` and `group4` as they are assigned to `group2`;
* he gets `group5` and `group6` as they are assigned to `group4`.

### Administration utilities
Using identifiers in `/etc/role` makes it hard to edit the file manually, that's why we have module administration utilities.
There are three utilities: `roleadd`, `roledel` и `rolelst`.

#### roleadd
```
roleadd [-s] ROLE [GROUP*]
```

Adds a role (if not exists) and assigns privileges and roles to it.

`ROLE` is a role name. Must match an existing group name.

`GROUP` is a name of role or a privilege.

When used with `-s` switch the groups are set; by default groups are appended.

#### roledel
```
roledel ROLE [GROUP*]
```
or
```
roledel -r ROLE
```

Used to delete privileges from roles and to delete roles themselves (second form).

#### rolelst
```
rolelst
```

Shows a nice output of `/etc/role`, converting identifiers into readable names.
