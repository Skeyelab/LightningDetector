# Scripts Directory Organization

This directory contains all project scripts organized by purpose.

## Directory Structure

### `ci/` - Continuous Integration Scripts
Scripts used in CI/CD pipelines for testing and quality assurance:
- `run_tests.sh` - Comprehensive unit testing script
- `run_static_analysis.sh` - Static code analysis with various tools
- `run_tidy.sh` - Clang-tidy code formatting and linting

### `dev/` - Development and Deployment Scripts
Scripts for local development and deployment workflows:
- `flash_both.sh` - Flash firmware to both sender and receiver devices
- `create_release.sh` - Create release packages and artifacts

### `optimize/` - Optimization and Analysis Scripts
Scripts for performance optimization and code analysis:
- `optimize_clang_tidy_cache.sh` - Optimize clang-tidy caching for faster builds

## Legacy Files
- `test_release_script.sh` - Test script for release validation
- `web_flasher_template.html` - Template for web flasher interface

## Usage

All scripts should be run from the project root directory. For example:
```bash
# Run tests
./scripts/ci/run_tests.sh

# Flash devices
./scripts/dev/flash_both.sh

# Create a release
./scripts/dev/create_release.sh
```

## CI Integration

The CI workflows reference these scripts and will automatically trigger when scripts are modified.