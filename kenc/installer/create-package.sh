#!/bin/bash

set -e

# KenC# Installation Script
VERSION="1.0.0"
PACKAGE_NAME="kenc-$VERSION"

# Create package directory
mkdir -p $PACKAGE_NAME
mkdir -p $PACKAGE_NAME/bin
mkdir -p $PACKAGE_NAME/include
mkdir -p $PACKAGE_NAME/examples
mkdir -p $PACKAGE_NAME/docs

# Copy compiler
cp -r ~/kenc/compiler/bin/kenc $PACKAGE_NAME/bin/
cp -r ~/kenc/compiler/*.h $PACKAGE_NAME/include/

# Copy IDE if compiled
if [ -f ~/kenc/ide/bin/kenc-ide ]; then
    cp -r ~/kenc/ide/bin/kenc-ide $PACKAGE_NAME/bin/
fi

# Copy examples
cp -r ~/kenc/examples/* $PACKAGE_NAME/examples/

# Copy documentation
cp -r /mnt/d/KenuxOS/KenC#/docs/* $PACKAGE_NAME/docs/

# Create installation script
cat > $PACKAGE_NAME/install.sh << 'EOF'
#!/bin/bash

set -e

echo "KenC# Installation Script"
echo "========================"

# Check for root privileges
if [ "$(id -u)" != "0" ]; then
    echo "Error: This script requires root privileges"
    exit 1
fi

# Create directories
mkdir -p /usr/local/bin
mkdir -p /usr/local/include/kenc
mkdir -p /usr/local/share/kenc/examples
mkdir -p /usr/local/share/kenc/docs

# Install compiler
echo "Installing KenC# compiler..."
cp bin/kenc /usr/local/bin/
chmod +x /usr/local/bin/kenc

# Install headers
echo "Installing header files..."
cp include/*.h /usr/local/include/kenc/

# Install IDE if present
if [ -f bin/kenc-ide ]; then
    echo "Installing KenC# IDE..."
    cp bin/kenc-ide /usr/local/bin/
    chmod +x /usr/local/bin/kenc-ide
fi

# Install examples
echo "Installing examples..."
cp -r examples/* /usr/local/share/kenc/examples/

# Install documentation
echo "Installing documentation..."
cp -r docs/* /usr/local/share/kenc/docs/

echo ""
echo "KenC# installation completed successfully!"
echo ""
echo "Usage:"
echo "  kenc <source_file.kenp>  - Compile KenC# program"
echo "  kenc-ide                 - Launch KenC# IDE (if installed)"
echo ""
echo "Examples are available in /usr/local/share/kenc/examples/"
echo "Documentation is available in /usr/local/share/kenc/docs/"
EOF

# Create uninstall script
cat > $PACKAGE_NAME/uninstall.sh << 'EOF'
#!/bin/bash

set -e

echo "KenC# Uninstallation Script"
echo "=========================="

# Check for root privileges
if [ "$(id -u)" != "0" ]; then
    echo "Error: This script requires root privileges"
    exit 1
fi

# Remove compiler
echo "Removing KenC# compiler..."
rm -f /usr/local/bin/kenc

# Remove IDE if present
if [ -f /usr/local/bin/kenc-ide ]; then
    echo "Removing KenC# IDE..."
    rm -f /usr/local/bin/kenc-ide
fi

# Remove headers
echo "Removing header files..."
rm -rf /usr/local/include/kenc

# Remove examples and documentation
echo "Removing examples and documentation..."
rm -rf /usr/local/share/kenc

echo ""
echo "KenC# uninstallation completed successfully!"
EOF

# Make scripts executable
chmod +x $PACKAGE_NAME/install.sh
chmod +x $PACKAGE_NAME/uninstall.sh

# Create README
cat > $PACKAGE_NAME/README.md << 'EOF'
# KenC# Programming Language

KenC# is a programming language for KenuxOS, with syntax similar to C. It compiles .kenp source files to .ken binary files.

## Features

- C-like syntax
- Integration with KenuxOS APIs
- Graphical IDE with preview support
- Cross-platform compilation

## Installation

### Linux/WSL

1. Extract the package:
   ```bash
   tar -xzf kenc-1.0.0.tar.gz
   cd kenc-1.0.0
   ```

2. Run the installation script:
   ```bash
   sudo ./install.sh
   ```

### Windows

Use WSL to install and run KenC#.

## Usage

### Command-line Compiler

```bash
# Compile a KenC# program
kenc hello.kenp

# Run the compiled program
./hello.ken
```

### Graphical IDE

```bash
# Launch the IDE
kenc-ide
```

## Examples

- `hello.kenp` - Basic hello world program
- `calc.kenp` - Simple calculator
- `gui.kenp` - GUI application example

## Documentation

- `language_spec.md` - KenC# language specification
- `API.md` - KenuxOS API documentation

## Uninstallation

Run the uninstall script:

```bash
sudo ./uninstall.sh
```
EOF

# Create package tarball
tar -czf $PACKAGE_NAME.tar.gz $PACKAGE_NAME

# Clean up
rm -rf $PACKAGE_NAME

echo ""
echo "KenC# installation package created successfully!"
echo "Package: $PACKAGE_NAME.tar.gz"
echo ""
echo "To install:"
echo "1. Extract the package: tar -xzf $PACKAGE_NAME.tar.gz"
echo "2. Run: sudo ./$PACKAGE_NAME/install.sh"
echo ""
echo "To uninstall:"
echo "1. Run: sudo ./$PACKAGE_NAME/uninstall.sh"
