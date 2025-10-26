# GitHub Actions CI/CD Documentation

This directory contains the complete CI/CD setup for the Modern C++ Template project.

## 📖 Documentation Index

### 🚀 Quick Start
**[QUICK_START.md](QUICK_START.md)** - Start here! Get your CI running in 5 minutes.

### 📋 Setup Information
**[CI_SETUP.md](CI_SETUP.md)** - Comprehensive setup summary and configuration details.

### 📊 Workflow Details
**[workflows/README.md](workflows/README.md)** - Detailed documentation of all CI workflows, local commands, and troubleshooting.

### 🔄 Migration Guide
**[MIGRATION_NOTES.md](MIGRATION_NOTES.md)** - Information about old workflow files and migration path.

### 📈 Visual Documentation
**[CI_FLOW.md](CI_FLOW.md)** - Visual diagrams showing CI pipeline flows and processes.

## 🎯 Choose Your Path

### "I just want it to work"
→ Read [QUICK_START.md](QUICK_START.md) (5 minutes)

### "I want to understand everything"
→ Read in order:
1. [QUICK_START.md](QUICK_START.md)
2. [CI_SETUP.md](CI_SETUP.md)
3. [workflows/README.md](workflows/README.md)
4. [CI_FLOW.md](CI_FLOW.md)

### "I need to debug a CI failure"
→ [workflows/README.md](workflows/README.md) → Troubleshooting section

### "I have old workflows"
→ [MIGRATION_NOTES.md](MIGRATION_NOTES.md)

### "I want visual explanations"
→ [CI_FLOW.md](CI_FLOW.md)

## 📁 Directory Structure

```
.github/
├── README.md                   # This file - Documentation index
├── QUICK_START.md              # 5-minute getting started guide
├── CI_SETUP.md                 # Complete setup summary
├── CI_FLOW.md                  # Visual flow diagrams
├── MIGRATION_NOTES.md          # Legacy workflow migration
│
└── workflows/
    ├── README.md               # Detailed workflow documentation
    ├── ci.yml                  # Main CI pipeline
    └── quick-check.yml         # Fast PR checks
```

## 🔧 Workflows Overview

### Main CI Pipeline (`ci.yml`)
Comprehensive testing across multiple platforms and configurations.

**Jobs:**
- ✅ Build Matrix (Ubuntu, macOS, Windows)
- ✅ Static Analysis (Clang-Tidy)
- ✅ Code Coverage (Codecov)
- ✅ Formatting Check
- ✅ Address Sanitizer

**Triggers:** Push to main/master/develop, Pull Requests

### Quick Check (`quick-check.yml`)
Fast feedback for pull requests.

**Jobs:**
- ✅ Quick Build (Ubuntu Debug)
- ✅ Format Check

**Triggers:** Pull Requests only

## 🛠️ Tools Provided

### Local CI Test Script
```bash
./scripts/test-ci-locally.sh
```
Simulates CI checks locally before pushing.

### Common Commands
```bash
# Format code
make format

# Run tests
cd build && ctest -C Debug -VV

# Code coverage
make coverage

# Build with static analysis
cmake -B build -DProject_ENABLE_CLANG_TIDY=ON
```

## 📊 CI Status

Check CI status in:
- **GitHub Actions Tab**: All workflow runs
- **Pull Request**: Status checks at bottom
- **README Badges**: Build status badges

## 🎓 Key Features

### Multi-Platform Testing
| Platform | Compiler | Status |
|----------|----------|--------|
| Ubuntu | GCC | ✅ Tested |
| Ubuntu | Clang | ✅ Tested |
| macOS | Clang | ✅ Tested |
| Windows | MSVC | ✅ Tested |

### Quality Assurance
- **Static Analysis**: Clang-Tidy finds bugs automatically
- **Code Coverage**: Track test coverage over time
- **Formatting**: Enforce consistent style
- **Memory Safety**: ASan detects memory errors

### Developer Experience
- **Fast Feedback**: Quick Check runs in ~3 minutes
- **Local Testing**: Test before pushing with script
- **Parallel Jobs**: All tests run simultaneously
- **Clear Errors**: Detailed logs for failures

## 📝 Best Practices

### Before Every Commit
1. ✅ Format code: `make format`
2. ✅ Run tests: `ctest -C Debug -VV`
3. ✅ (Optional) Run local CI: `./scripts/test-ci-locally.sh`

### Creating Pull Requests
1. ✅ Push to feature branch
2. ✅ Wait for Quick Check (~3 minutes)
3. ✅ Fix any failures
4. ✅ Request review after passing

### Merging
1. ✅ All checks pass (green ✓)
2. ✅ Code reviewed
3. ✅ Branch up to date
4. ✅ Merge!

## 🐛 Common Issues

### Formatting Check Fails
```bash
make format
git add -A && git commit -m "style: apply formatting"
```

### Build Fails
Check GitHub Actions logs → Click job → Read error

### Tests Fail
Run locally: `cd build && ctest -C Debug -VV`

### Coverage Upload Fails
Add `CODECOV_TOKEN` secret (private repos only)

## 📖 Additional Resources

- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [GoogleTest Documentation](https://google.github.io/googletest/)
- [Clang-Tidy Checks](https://clang.llvm.org/extra/clang-tidy/)
- [Codecov Documentation](https://docs.codecov.com/)

## 🔄 Updates

This CI setup is actively maintained. Check timestamps on documentation files for last update dates.

### Latest Updates
- **2025-10-26**: Initial CI pipeline setup
  - Main CI workflow with matrix builds
  - Quick check workflow for PRs
  - Comprehensive documentation
  - Local testing script

## 🆘 Getting Help

### For CI Issues
1. Check [workflows/README.md](workflows/README.md) Troubleshooting section
2. Review GitHub Actions logs
3. Run `./scripts/test-ci-locally.sh` to reproduce
4. Check workflow YAML files for configuration

### For Setup Questions
1. Read [QUICK_START.md](QUICK_START.md)
2. Review [CI_SETUP.md](CI_SETUP.md)
3. Check [MIGRATION_NOTES.md](MIGRATION_NOTES.md) for old workflow info

### For Understanding Flows
1. See [CI_FLOW.md](CI_FLOW.md) for visual diagrams
2. Read workflow files with inline comments

## ✅ Quick Checklist

Setup complete when:
- [ ] README badges updated with your username
- [ ] Pushed to GitHub
- [ ] Workflows run successfully
- [ ] All jobs pass (green ✓)
- [ ] Coverage reports upload
- [ ] Branch protection configured (optional)
- [ ] Old workflows removed (optional)

## 🎉 Success!

You now have a professional-grade CI/CD pipeline for your C++ project!

**Key Benefits:**
- 🚀 Automated testing on every commit
- 🛡️ Multiple layers of quality checks
- ⚡ Fast feedback with Quick Check
- 🔧 Easy local development workflow
- 📊 Coverage tracking over time
- 🌍 Multi-platform support

---

**Ready to dive in?** Start with [QUICK_START.md](QUICK_START.md)!

**Need details?** See [workflows/README.md](workflows/README.md)!

**Want visuals?** Check [CI_FLOW.md](CI_FLOW.md)!

