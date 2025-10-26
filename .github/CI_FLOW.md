# CI Pipeline Flow Diagram

## Overview

This document visualizes how the CI pipelines work in this project.

## Trigger Flow

```mermaid
graph TD
    A[Developer Commits Code] --> B{Push or PR?}
    B -->|Push to main/master/develop| C[Full CI Pipeline]
    B -->|Pull Request| D[Quick Check Pipeline]
    B -->|Pull Request| E[Full CI Pipeline]
    
    C --> F[Build Matrix Jobs]
    C --> G[Static Analysis]
    C --> H[Code Coverage]
    C --> I[Formatting Check]
    C --> J[ASan Tests]
    
    D --> K[Quick Build Ubuntu]
    D --> L[Format Check]
    
    F --> M[Ubuntu GCC Release]
    F --> N[Ubuntu Clang Release]
    F --> O[macOS Release]
    F --> P[Windows MSVC]
    F --> Q[Ubuntu Debug]
    
    M --> R{All Pass?}
    N --> R
    O --> R
    P --> R
    Q --> R
    G --> R
    H --> R
    I --> R
    J --> R
    K --> R
    L --> R
    
    R -->|Yes| S[✓ CI Success]
    R -->|No| T[✗ CI Failure]
    
    S --> U[Ready to Merge]
    T --> V[Fix Issues]
    V --> A
```

## Detailed Job Flow

### Main CI Pipeline (ci.yml)

```mermaid
graph LR
    subgraph "Build Matrix"
        A1[Ubuntu GCC]
        A2[Ubuntu Clang]
        A3[macOS]
        A4[Windows]
        A5[Ubuntu Debug]
    end
    
    subgraph "Quality Checks"
        B1[Static Analysis]
        B2[Code Coverage]
        B3[Formatting]
        B4[ASan Tests]
    end
    
    A1 --> C[Tests Pass?]
    A2 --> C
    A3 --> C
    A4 --> C
    A5 --> C
    B1 --> C
    B2 --> D[Upload to Codecov]
    B3 --> C
    B4 --> C
    
    D --> C
    C -->|All Pass| E[✓ CI Success]
    C -->|Any Fail| F[✗ CI Failure]
```

## Build Step Breakdown

### Individual Job Flow

```mermaid
sequenceDiagram
    participant GH as GitHub
    participant VM as CI Runner
    participant Build as Build System
    participant Tests as Test Suite
    
    GH->>VM: Trigger workflow
    VM->>VM: Checkout code
    VM->>VM: Setup environment
    VM->>Build: Configure CMake
    Build->>Build: Generate build files
    VM->>Build: Build project
    Build->>Build: Compile sources
    Build->>Build: Link libraries
    Build-->>VM: Build artifacts
    VM->>Tests: Run CTest
    Tests->>Tests: Execute all tests
    Tests-->>VM: Test results
    VM->>GH: Report status
    
    alt All tests pass
        GH->>GH: Mark as ✓ Success
    else Any test fails
        GH->>GH: Mark as ✗ Failure
    end
```

## Quick Check Pipeline Flow

```mermaid
graph TD
    A[PR Created/Updated] --> B[Quick Check Triggered]
    
    B --> C[Quick Build Job]
    B --> D[Format Check Job]
    
    C --> E[Configure CMake Debug]
    E --> F[Build with GCC]
    F --> G[Run All Tests]
    
    D --> H[Check all .hpp/.cpp files]
    H --> I{Format OK?}
    
    G --> J{Tests Pass?}
    
    I -->|Yes| K[✓ Format OK]
    I -->|No| L[✗ Run make format]
    
    J -->|Yes| M[✓ Tests Pass]
    J -->|No| N[✗ Fix Failing Tests]
    
    K --> O{Both OK?}
    M --> O
    L --> O
    N --> O
    
    O -->|Yes| P[✓ Ready for Review]
    O -->|No| Q[✗ Needs Fixes]
```

## Platform-Specific Workflow

```mermaid
graph TB
    subgraph "Linux (Ubuntu)"
        L1[Install: gcc/clang, cmake, ninja]
        L2[Configure CMake]
        L3[Build with Ninja]
        L4[Run Tests]
        L1 --> L2 --> L3 --> L4
    end
    
    subgraph "macOS"
        M1[Install: cmake, ninja via brew]
        M2[Configure CMake]
        M3[Build with Ninja]
        M4[Run Tests]
        M1 --> M2 --> M3 --> M4
    end
    
    subgraph "Windows"
        W1[Install: cmake, ninja via choco]
        W2[Configure CMake]
        W3[Build with Ninja]
        W4[Run Tests]
        W1 --> W2 --> W3 --> W4
    end
    
    L4 --> R[Aggregate Results]
    M4 --> R
    W4 --> R
```

## Code Coverage Flow

```mermaid
graph LR
    A[Build with Coverage] --> B[Run Tests]
    B --> C[Collect Coverage Data]
    C --> D[Generate coverage.info]
    D --> E[Filter System Files]
    E --> F[Upload to Codecov]
    F --> G[View Coverage Report]
    G --> H[Coverage Badge Updated]
```

## Static Analysis Flow

```mermaid
graph TD
    A[Configure with Clang-Tidy] --> B[Build Project]
    B --> C[Clang-Tidy Analyzes Each File]
    C --> D{Issues Found?}
    D -->|No| E[✓ Analysis Pass]
    D -->|Yes| F[Report Warnings/Errors]
    F --> G{Errors Critical?}
    G -->|No| H[⚠ Pass with Warnings]
    G -->|Yes| I[✗ Analysis Fail]
```

## Complete PR Workflow

```mermaid
stateDiagram-v2
    [*] --> Draft: Create PR
    Draft --> InReview: Mark ready
    
    InReview --> QuickCheck: Trigger Quick Check
    InReview --> FullCI: Trigger Full CI
    
    QuickCheck --> QuickResults
    FullCI --> FullResults
    
    QuickResults --> CheckStatus: Format & Build OK?
    FullResults --> CheckStatus: All Jobs Pass?
    
    CheckStatus --> AllPass: ✓
    CheckStatus --> SomeFail: ✗
    
    SomeFail --> FixRequired: Need Changes
    FixRequired --> InReview: Push Fixes
    
    AllPass --> ReadyToMerge: All Checks ✓
    ReadyToMerge --> Merged: Approve & Merge
    Merged --> [*]
```

## Time Estimates

### Quick Check Pipeline
```
┌─────────────────────────────────┐
│ Quick Build:      ~2-3 minutes  │
│ Format Check:     ~30 seconds   │
│ Total:            ~2-3 minutes  │
└─────────────────────────────────┘
```

### Full CI Pipeline
```
┌─────────────────────────────────────────┐
│ Build Matrix (parallel):                │
│   - Ubuntu GCC:        ~3-4 minutes     │
│   - Ubuntu Clang:      ~3-4 minutes     │
│   - macOS:             ~4-5 minutes     │
│   - Windows:           ~5-6 minutes     │
│   - Ubuntu Debug:      ~3-4 minutes     │
│                                          │
│ Static Analysis:       ~4-5 minutes     │
│ Code Coverage:         ~4-5 minutes     │
│ Formatting:            ~30 seconds      │
│ ASan:                  ~4-5 minutes     │
│                                          │
│ Total (parallel):      ~6-8 minutes     │
└─────────────────────────────────────────┘
```

## Failure Handling

```mermaid
graph TD
    A[CI Failure Detected] --> B{Which Job Failed?}
    
    B -->|Build| C[Check Compilation Errors]
    B -->|Tests| D[Check Test Logs]
    B -->|Format| E[Run make format]
    B -->|Coverage| F[Check Coverage Config]
    B -->|Static Analysis| G[Review Clang-Tidy Output]
    
    C --> H[Fix Code]
    D --> H
    E --> I[Commit Formatted Code]
    F --> J[Update Coverage Settings]
    G --> K[Fix or Suppress Warnings]
    
    H --> L[Push Fix]
    I --> L
    J --> L
    K --> L
    
    L --> M[CI Reruns Automatically]
    M --> N{Fixed?}
    N -->|Yes| O[✓ Continue]
    N -->|No| A
```

## Best Practices Flow

```mermaid
graph TD
    A[Before Committing] --> B[Run Local Tests]
    B --> C[Run make format]
    C --> D[Review Changes]
    D --> E[Commit]
    E --> F[Create PR]
    F --> G[Quick Check Runs]
    G --> H{Quick Check Pass?}
    H -->|No| I[Fix Locally]
    I --> E
    H -->|Yes| J[Request Review]
    J --> K[Full CI Runs]
    K --> L{Full CI Pass?}
    L -->|No| M[Investigate CI Logs]
    M --> I
    L -->|Yes| N[Approve & Merge]
```

## Local Testing Recommendation

```mermaid
graph LR
    A[Local Development] --> B[scripts/test-ci-locally.sh]
    B --> C[Format Check]
    B --> D[Build Test]
    B --> E[Unit Tests]
    B --> F[Optional: Static Analysis]
    B --> G[Optional: ASan]
    
    C --> H{All Pass?}
    D --> H
    E --> H
    F --> H
    G --> H
    
    H -->|Yes| I[✓ Ready to Push]
    H -->|No| J[Fix Issues Locally]
    J --> A
    I --> K[git push]
    K --> L[CI Validates]
```

---

**Note:** All diagrams are in Mermaid format and will render automatically on GitHub and in compatible viewers.

