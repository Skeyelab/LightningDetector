#!/bin/bash

# SBT PIO Web Flasher Deployment Script
# This script builds the web flasher and prepares it for GitHub Pages deployment

set -e

echo "🚀 Building SBT PIO Web Flasher..."

# Build the project
npm run build

echo "📁 Preparing deployment files..."

# Create deployment directory
DEPLOY_DIR="../gh-pages"
mkdir -p $DEPLOY_DIR

# Copy built files
cp -r dist/* $DEPLOY_DIR/

# Copy firmware files and manifests (these should already be in dist from the build)
if [ -f "dist/sender_firmware_v1.0.0.bin" ]; then
    echo "✅ Firmware files already in dist folder"
else
    echo "⚠️  Firmware files not found in dist, copying from root..."
    cp sender_firmware_v1.0.0.bin dist/
    cp receiver_firmware_v1.0.0.bin dist/
    cp sender_manifest.json dist/
    cp receiver_manifest.json dist/
    # Copy updated dist contents
    cp -r dist/* $DEPLOY_DIR/
fi

# Copy README for GitHub Pages
cp README.md $DEPLOY_DIR/

echo "✅ Deployment files prepared in $DEPLOY_DIR"
echo ""
echo "📋 Next steps:"
echo "1. Commit and push the contents of $DEPLOY_DIR to your gh-pages branch"
echo "2. Or copy the contents to your GitHub Pages deployment directory"
echo "3. Ensure your repository has GitHub Pages enabled"
echo ""
echo "🔗 Your web flasher will be available at:"
echo "   https://yourusername.github.io/LightningDetector/"
echo ""
echo "📁 Files deployed:"
ls -la $DEPLOY_DIR/
