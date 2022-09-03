# shine
A small ruby project creator

## Initialization
To create a new ruby project, use the ```init``` subcommand

```
$ shine init --help
SYNOPSIS

    shine init --type <exec | lib> [--set-version <VALUE>]
        [(--summary|-s) <VALUE>] [(--description|-d) <VALUE>]
        [(--authors|-a) <VALUE>...] [(--email|-e) <VALUE>]
        [(--homepage|-h) <VALUE>] <Project name>

DESCRIPTION

    Init a ruby project

OPTIONS

    --authors VALUE, -a VALUE
        authors

    --description VALUE, -d VALUE
        Project description

    --email VALUE, -e VALUE
        email

    --homepage VALUE, -h VALUE
        Project homepage

    --set-version VALUE
        Project version

    --summaary VALUE, -s VALUE
        Project summary

    --type exec | lib

    Project name
```

The command, if sucessful will create the following folder structure
``` 
$ shine init --type exec project_name

.
└── project_name
    ├── README
    ├── Rakefile
    ├── bin
    │   └── project_name
    ├── lib
    │   └── project_name.rb
    ├── project_name.gemspec
    └── test
        └── test_project_name.rb

```
