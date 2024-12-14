module.exports = {
    rules: {
        'header-case': [1, 'always', 'lower-case'],
        'header-full-stop': [0, 'always', ''],
        'header-max-length': [2, 'always', 72],
        'header-min-length': [2, 'always', 1],

        'type-enum': [
            2,
            'always',
            // Also edit types in /README.md
            ['fix', 'feat', 'refactor', 'perf', 'build', 'revert'],
        ],
        'type-case': [2, 'always', 'lower-case'],
        'type-empty': [2, 'never', false],

        // Also edit scopes in /README.md
        'scope-enum': [2, 'always', ['*', 'repo', 'backend', 'desk-cl']],
        'scope-case': [2, 'always', 'kebab-case'],
        'scope-empty': [2, 'never', false],

        'subject-case': [1, 'always', 'lower-case'],
        'subject-empty': [2, 'never', false],

        'body-full-stop': [0, 'always', ''],
        'body-leading-blank': [2, 'always', true],
        'body-empty': [1, 'never', false],
        'body-case': [1, 'always', ['lower-case', 'sentence-case']],

        'footer-leading-blank': [2, 'always', true],
        'footer-empty': [0, 'never', false],
    },
}
