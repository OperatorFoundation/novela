#!/usr/bin/env fish

function remove_git_submodule
    # Check if we got an argument
    if test (count $argv) -eq 0
        echo "Usage: remove_git_submodule <submodule-directory>"
        echo "Example: remove_git_submodule libs/some-library"
        return 1
    end

    set submodule_path $argv[1]

    # Remove trailing slash if present
    set submodule_path (string trim --right --chars=/ $submodule_path)

    # Check if we're in a git repository
    if not git rev-parse --git-dir >/dev/null 2>&1
        echo "Error: Not in a git repository"
        return 1
    end

    # Check if the submodule exists
    if not test -d $submodule_path
        echo "Error: Directory '$submodule_path' does not exist"
        return 1
    end

    # Check if it's actually a submodule
    if not git submodule status $submodule_path >/dev/null 2>&1
        echo "Error: '$submodule_path' is not a git submodule"
        return 1
    end

    echo "Removing git submodule: $submodule_path"

    # Step 1: Deinitialize the submodule
    echo "→ Deinitializing submodule..."
    git submodule deinit -f $submodule_path
    if test $status -ne 0
        echo "Error: Failed to deinitialize submodule"
        return 1
    end

    # Step 2: Remove the submodule from .git/modules
    echo "→ Removing from .git/modules..."
    rm -rf .git/modules/$submodule_path
    if test $status -ne 0
        echo "Error: Failed to remove .git/modules/$submodule_path"
        return 1
    end

    # Step 3: Remove the submodule from the working tree
    echo "→ Removing from git index..."
    git rm -f $submodule_path
    if test $status -ne 0
        echo "Error: Failed to remove submodule from git"
        return 1
    end

    # Step 4: Check if .gitmodules is now empty and remove if so
    if test -f .gitmodules
        set gitmodules_content (string trim (cat .gitmodules))
        if test -z "$gitmodules_content"
            echo "→ Removing empty .gitmodules file..."
            git rm .gitmodules
        end
    end

    echo "✓ Submodule '$submodule_path' successfully removed"
    echo ""
    echo "Don't forget to commit these changes:"
    echo "  git commit -m \"Remove submodule $submodule_path\""
end

# Run the function with arguments passed to the script
remove_git_submodule $argv
