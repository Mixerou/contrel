<h1 align="center">
    <span>Contrel — from the words <b>Contr</b>ol and Hot<b>el</b></span>
    <br>
    <span>🏨 Build a Hotel Empire 🏨</span>
</h1>

## Content

- [Commit Lint](#commit-lint)
  - [Explanation](#explanation)
  - [Installation](#installation)
  - [Usage](#usage)

## Commit Lint

To keep the repository clean
and to make it easy for new developers to enter the project,
we use `commitlint`.

### Explanation

All commits in this repo look like this

```text
<Header>
<BLANK LINE>
<Body>
<BLANK LINE>
<BLANK LINE>
<Footer>
```

Header

```text
type(scope)!: subject
```

#### Type

Type is super short info about this commit.
Use it to communicate your intent to the project developers.

- `fix` - patches a bug in the codebase

- `feat` - introduces a new feature to the codebase

- `refactor` - a code change that neither fixes a bug nor adds a feature

- `perf` - a code change that improves performance

- `build` - changes that affect the build system or external dependencies

- `revert` - revert commits

#### Scope

- `*` - it selects all available scopes

- `repo` - changes that not affect project modules (example: ci, readme, etc.)

- `backend` - changes that effects [`Backend`](./backend) module

- `desk-cl` - changes that effects [`Desktop Client`](./desktop-client) module

#### Exclamation mark

If a commit has a footer `BREAKING CHANGE:`,
or appends a `!` after the type/scope,
it introduces breaking codebase changes.

#### Subject

Summary in present tense. Not capitalized. No period at the end.

#### Revert Commits

If the commit reverts a previous commit, it should have `revert` type,
followed by the header of the reverted commit.

The content of the commit message body should contain:

- information about the SHA of the commit being reverted in the following format:
`This reverts commit <SHA>`
- a clear description of the reason for reverting the commit message.

### Installation

Use the following commands to start using it.

> You must have [Bun](https://bun.sh/) installed

```bash
$ bun --bun install -g @commitlint/cli

$ bun --bun x husky@8.x.x install

$ bun --bun x husky@8.x.x add .husky/commit-msg "bun --bun x --no -- commitlint --edit ${1}"
```

You can use old-school `Node JS` 😎, but you'll need to adapt the commands above to do so.

### Usage

```bash
$ git commit -m "refactor(repo): fix typo in README file"
```
