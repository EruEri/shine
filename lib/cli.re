let shine_version = "0.1.0";

module Init_Command = {
  type project_target =
    | Exec
    | Lib;

  type arguments = {
    proj_type: project_target,
    version: option(string),
    summary: option(string),
    description: option(string),
    authors: list(string),
    email: option(string),
    homepage: option(string),
    name: string,
  };

  let create_main_dir = (arguments, ()) => {
    Sys.mkdir(arguments.name, 0o764);
  };

  let create_inner_direct = (arguments, ()) => {
    let project_name = arguments.name;
    let proj_type = arguments.proj_type;
    Sys.mkdir(project_name ++ Filename.dir_sep ++ "lib", 0o764);
    Sys.mkdir(project_name ++ Filename.dir_sep ++ "test", 0o764);
    switch (proj_type) {
    | Lib => ()
    | Exec => Sys.mkdir(project_name ++ Filename.dir_sep ++ "bin", 0o764)
    };
  };

  let gemspec_parameters = arguments => {
    Printf.sprintf(
      "Gem::Specification.new do |s|\n%s%s%s%s%s%s%send",
      Printf.sprintf("\ts.name        = '%s'\n", arguments.name),
      arguments.version
      |> Option.map(Printf.sprintf("\ts.version     = '%s'\n"))
      |> Option.value(~default=""),
      arguments.summary
      |> Option.map(Printf.sprintf("\ts.summary     = '%s'\n"))
      |> Option.value(~default=""),
      arguments.description
      |> Option.map(Printf.sprintf("\ts.description = '%s'\n"))
      |> Option.value(~default=""),
      arguments.authors
      |> String.concat(", ")
      |> (
        s =>
          if (s == "") {
            s;
          } else {
            Printf.sprintf("\ts.summary     = '%s'\n", s);
          }
      ),
      arguments.email
      |> Option.map(Printf.sprintf("\ts.email       = '%s'\n"))
      |> Option.value(~default=""),
      arguments.homepage
      |> Option.map(Printf.sprintf("\ts.homepage    = '%s'\n"))
      |> Option.value(~default=""),
    );
  };

  let create_file = (arguments, ()) => {
    let project_name = arguments.name;
    let proj_type = arguments.proj_type;
    let lib_file =
      open_out(
        project_name
        ++ Filename.dir_sep
        ++ "lib"
        ++ Filename.dir_sep
        ++ project_name
        ++ ".rb",
      );
    close_out(lib_file);
    let test_file =
      open_out(
        project_name
        ++ Filename.dir_sep
        ++ "test"
        ++ Filename.dir_sep
        ++ "test_"
        ++ project_name
        ++ ".rb",
      );
    close_out(test_file);
    switch (proj_type) {
    | Lib => ()
    | Exec =>
      let bin_file =
        open_out(
          project_name
          ++ Filename.dir_sep
          ++ "bin"
          ++ Filename.dir_sep
          ++ project_name,
        );

      output_string(bin_file, "#! /usr/bin/env ruby\n\nputs \"hello world\"");
      close_out(bin_file);
    };
    project_name ++ Filename.dir_sep ++ "README" |> open_out |> close_out;
    project_name ++ Filename.dir_sep ++ "Rakefile" |> open_out |> close_out;
    let gemspec_file = open_out(project_name ++ Filename.dir_sep ++ project_name ++ ".gemspec");
    arguments |> gemspec_parameters |> output_string(gemspec_file);
    close_out(gemspec_file);
  };

  let exec = (arguments: arguments, ()) =>
    try(
      create_main_dir(arguments, ())
      |> create_inner_direct(arguments)
      |> create_file(arguments)
      |> Result.ok
    ) {
    | Sys_error(s) => Result.error(s)
    };
};

module Run_Command = {
  type arguments = {
    file_name: string,
    ruby_arguments: string
  }

  let is_execuatable = () => Sys.file_exists("bin")

  let lib_folder_exist = () => Sys.file_exists("lib")

  let exec = (arguments: arguments, ()) => {
    if (! is_execuatable()) { 
      Result.error("No bin folder")
    } else {
      try (
        Sys.command(Printf.sprintf("ruby %s bin/%s %s", 
          lib_folder_exist() ? "-Ilib" : "",
          arguments.file_name,
          arguments.ruby_arguments)
          )
          |> (_ => Ok ())
      ) {
        | Sys_error(e) => Result.error(e)
      }
    }
  }
}

type shine_subcommand =
  | Init(Init_Command.arguments)
  | Run(Run_Command.arguments);

let commands = () => {
  open Init_Command;
  Clap.description("A small ruby project creator");

  let subcommans =
    Clap.subcommand([
      Clap.case(~description="Init a ruby project", "init", () => {
        let proj_type_typ =
          Clap.enum("type", [("exec", Exec), ("lib", Lib)]);
        let proj_type =
          Clap.mandatory(
            proj_type_typ,
            ~long="type",
            ~placeholder="exec | lib",
            (),
          );
        let version =
          Clap.optional_string(
            ~long="set-version",
            ~description="Project version",
            (),
          );
        let summary =
          Clap.optional_string(
            ~short='s',
            ~long="summary",
            ~description="Project summary",
            (),
          );
        let description =
          Clap.optional_string(
            ~short='d',
            ~long="description",
            ~description="Project description",
            (),
          );
        let authors =
          Clap.list_string(
            ~short='a',
            ~long="authors",
            ~description="authors",
            (),
          );
        let email =
          Clap.optional_string(
            ~short='e',
            ~long="email",
            ~description="email",
            (),
          );
        let homepage =
          Clap.optional_string(
            ~short='h',
            ~long="homepage",
            ~description="Project homepage",
            (),
          );
        let name =
          Clap.mandatory_string(~last=true, ~placeholder="Project name", ());

        Init({
          proj_type,
          version,
          summary,
          description,
          authors,
          email,
          homepage,
          name,
        });
      }),
      Clap.case(~description="Run a specific file", "run", () => {
        let file_name = Clap.mandatory_string(~last= false, ~placeholder="ruby file", ());
        let ruby_arguments = 
          Clap.list_string(~placeholder = "Arguments", ()) |> String.concat(" ")

          Run({
            file_name,
            ruby_arguments
          })
      })
    ]);
  ();

  Clap.close();

  subcommans;
};

let run = (shine_subcommand, ()) => {
  let error =
    switch (shine_subcommand) {
    | Init(arguments) => Init_Command.exec(arguments, ())
    | Run(arguments) => Run_Command.exec(arguments, ())
    };

  error
  |> Result.iter_error(s => {
       Printf.printf("An error occured : %s\n", s);
       exit(1);
     });
};
