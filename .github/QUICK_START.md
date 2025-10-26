# CI Quick Start Guide

Welcome! This guide will get you up and running with the new CI pipeline in minutes.

## 📋 What You Got

✅ **Automated Testing** - Multi-platform builds (Linux, macOS, Windows)  
✅ **Code Quality** - Static analysis with Clang-Tidy  
✅ **Code Coverage** - Automatic coverage reports  
✅ **Style Enforcement** - Consistent code formatting  
✅ **Memory Safety** - AddressSanitizer integration  
✅ **Fast Feedback** - Quick check for PRs (~3 minutes)  

## 🚀 Getting Started (5 minutes)

### Step 1: Update Badge URLs (1 minute)

Edit `README.md` and replace `teddymalhan` with your GitHub username:

```markdown
[![CI](https://github.com/teddymalhan/modern-cpp-template/workflows/CI/badge.svg)]...
```

### Step 2: Push to GitHub (1 minute)

```bash
git add .github/
git commit -m "ci: add comprehensive CI pipeline"
git push origin main
```

### Step 3: Verify Workflows Run (3 minutes)

1. Go to your GitHub repository
2. Click "Actions" tab
3. See workflows running
4. Wait for completion (~6-8 minutes)
5. Check for green checkmarks ✓

**That's it!** Your CI is now active.

## 💻 Daily Development Workflow

### Before Every Commit

```bash
# 1. Format your code
make format

# 2. Run tests locally
cd build && ctest -C Debug -VV

# 3. (Optional) Run full local CI simulation
./scripts/test-ci-locally.sh
```

### Creating a Pull Request

```bash
# 1. Create feature branch
git checkout -b feature/my-feature

# 2. Make changes and commit
git add .
git commit -m "feat: add new feature"

# 3. Push to GitHub
git push origin feature/my-feature

# 4. Create PR on GitHub
# Quick Check will run automatically (~3 minutes)

# 5. Wait for checks to pass
# Request review once green ✓
```

### If CI Fails

```bash
# 1. Check which job failed on GitHub Actions tab
# 2. Read the error logs
# 3. Fix locally
# 4. Push fix - CI reruns automatically
```

## 🔧 Common Commands

### Format Code
```bash
make format
# or
cmake --build build --target clang-format
```

### Run Tests
```bash
cd build && ctest -C Release -VV
```

### Build with Static Analysis
```bash
cmake -B build -DProject_ENABLE_CLANG_TIDY=ON
cmake --build build
```

### Code Coverage
```bash
make coverage
```

### Local CI Simulation
```bash
./scripts/test-ci-locally.sh
```

## 📁 What Was Added

```
.github/
├── workflows/
│   ├── ci.yml              # Main CI pipeline ⭐
│   ├── quick-check.yml     # Fast PR checks ⭐
│   └── README.md           # Workflow docs
├── CI_SETUP.md             # Setup summary
├── CI_FLOW.md              # Visual diagrams
├── MIGRATION_NOTES.md      # Old workflow notes
└── QUICK_START.md          # This file

scripts/
└── test-ci-locally.sh      # Local testing script ⭐

README.md                    # Updated with CI section
```

## 🎯 CI Jobs Explained

### Quick Check (Pull Requests)
- ⚡ **Fast**: 2-3 minutes
- 🎯 **Purpose**: Immediate feedback
- ✅ **Checks**: Build + format

### Main CI (All Commits)
- 🔄 **Comprehensive**: All platforms
- 🛡️ **Quality**: Multiple checks
- ⏱️ **Time**: 6-8 minutes (parallel)

### Jobs Breakdown:

| Job | What It Does | Time |
|-----|--------------|------|
| Ubuntu GCC | Build & test on Linux | 3-4 min |
| Ubuntu Clang | Alternative compiler | 3-4 min |
| macOS | Build & test on Mac | 4-5 min |
| Windows | Build & test on Windows | 5-6 min |
| Static Analysis | Find bugs with Clang-Tidy | 4-5 min |
| Code Coverage | Measure test coverage | 4-5 min |
| Formatting | Check code style | 30 sec |
| ASan | Memory error detection | 4-5 min |

**All jobs run in parallel** → Total: ~6-8 minutes

## 🐛 Troubleshooting

### "Formatting check failed"
```bash
make format
git add -A
git commit -m "style: apply formatting"
git push
```

### "Build failed on platform X"
Check the logs in GitHub Actions → Click job → Read error message

### "Tests timeout"
Check for infinite loops or deadlocks in your code

### "Coverage upload failed"
For private repos, add `CODECOV_TOKEN` secret:
1. Get token from codecov.io
2. GitHub → Settings → Secrets → New secret
3. Name: `CODECOV_TOKEN`

## 🎓 Learn More

- **Detailed Workflow Docs**: [workflows/README.md](.github/workflows/README.md)
- **Visual Flow Diagrams**: [CI_FLOW.md](CI_FLOW.md)
- **Migration Notes**: [MIGRATION_NOTES.md](MIGRATION_NOTES.md)
- **Full Setup Info**: [CI_SETUP.md](CI_SETUP.md)

## 💡 Pro Tips

### 1. Skip CI for Docs
```bash
git commit -m "docs: update README [skip ci]"
```

### 2. Test Locally First
Always run `./scripts/test-ci-locally.sh` before pushing

### 3. Monitor CI Time
If CI takes too long:
- Consider disabling optional checks
- Use Quick Check for rapid iteration
- Enable ccache (already configured)

### 4. Branch Protection
Settings → Branches → Add rule:
- ✅ Require status checks: `Quick Build and Test`
- ✅ Require status checks: `Format Check`

### 5. Review Coverage
Check Codecov dashboard after merging:
- Aim for 80%+ coverage
- Focus on critical code paths

## 📊 Status Badges

Add to your README:

```markdown
[![CI](https://github.com/USER/REPO/workflows/CI/badge.svg)](...)
[![Quick Check](https://github.com/USER/REPO/workflows/Quick%20Check/badge.svg)](...)
[![codecov](https://codecov.io/gh/USER/REPO/branch/main/graph/badge.svg)](...)
```

Already added ✅ (just update teddymalhan)

## 🧹 Cleanup (Optional)

Remove old workflow files (see [MIGRATION_NOTES.md](MIGRATION_NOTES.md)):

```bash
cd .github/workflows/
rm -f macos.yml ubuntu.yml windows.yml release.yml
git add -A
git commit -m "ci: remove legacy workflows"
```

## ✅ Checklist

- [ ] Updated README badge URLs
- [ ] Pushed to GitHub
- [ ] Verified workflows run successfully
- [ ] All jobs pass (green ✓)
- [ ] (Optional) Set up branch protection
- [ ] (Optional) Configure Codecov token (private repos)
- [ ] (Optional) Remove old workflow files
- [ ] Familiarized with `scripts/test-ci-locally.sh`
- [ ] Reviewed workflow documentation

## 🎉 Success Criteria

Your CI is working correctly when:

1. ✅ Green checkmarks on all jobs
2. ✅ Coverage reports upload to Codecov
3. ✅ PRs show status checks
4. ✅ Formatting enforced automatically
5. ✅ Tests run on every commit

## 🆘 Need Help?

1. Check [workflows/README.md](.github/workflows/README.md) for detailed docs
2. Review GitHub Actions logs for specific errors
3. Run `./scripts/test-ci-locally.sh` to reproduce issues
4. Check [CI_FLOW.md](CI_FLOW.md) for visual explanations

## 📝 Next Steps

**Immediate:**
1. Update README badges
2. Push and verify CI runs
3. Create a test PR

**Soon:**
1. Configure branch protection
2. Set up Codecov (if private)
3. Clean up old workflows

**Later:**
1. Customize CI for your needs
2. Add project-specific checks
3. Set up release automation

---

**Questions?** See the full documentation in `.github/workflows/README.md`

**Ready to go!** 🚀 Your CI pipeline is set up and ready for professional C++ development.

