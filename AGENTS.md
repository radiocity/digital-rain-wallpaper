# Agent Guidelines

Rules for anyone (human or AI agent) changing this repository.

## Commits

- Use [Conventional Commits](https://www.conventionalcommits.org/en/v1.0.0/).
- Commit message is a title only: `<type>(<optional scope>): <summary>`.
  No body, no footer.
- No AI attribution of any kind: no `Co-Authored-By` lines, no "Generated
  with" notes, in commits or anywhere else.

## Versioning

The mod version (`@version` in the mod metadata block) follows
[Semantic Versioning](https://semver.org/) with one project-specific rule:

- Breaking changes bump **MINOR**, never MAJOR.
- New features and other changes bump **MINOR**.
- Fixes bump **PATCH**.

Every published change must increase `@version`. Windhawk refuses updates that
don't.

## Changelog

- Record every version in [CHANGELOG.md](CHANGELOG.md), newest first.
- Group entries by Conventional Commits type: `Features`, `Bug Fixes`,
  `Performance Improvements`, `Documentation`, and so on. Mark breaking changes
  under `BREAKING CHANGES`.
- Write each entry in the form `scope: description`, the same as the commit
  title.

## Code style

- Follow the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html).
- The only exceptions are names fixed by external APIs, such as the Windhawk
  callbacks (`Wh_ModInit`, `Wh_ModUninit`, ...) and Win32 or Direct3D types.
- The mod must stay a single `.wh.cpp` file so it can be pasted into the
  Windhawk editor and submitted to
  [windhawk-mods](https://github.com/ramensoftware/windhawk-mods) as is.
- Settings must stay compatible with Windhawk 1.7.3. Put Windhawk 2.0 settings
  annotations on `#! ` lines.

## Third-party code and licenses

- Credit the author of any third-party code we take, adapt or translate in the
  "Credits" section of [README.md](README.md), with a link to the source and its
  license.
- Don't break licenses:
  - Keep the original copyright and license notices.
  - Mark modified files as modified, with a date.
  - Keep the project license compatible with everything it includes.
  - The mod includes GPL-3.0 code, so the whole mod stays GPL-3.0.
- Repeat the attribution in the mod's metadata or readme block, because the
  `.wh.cpp` file is distributed on its own.
