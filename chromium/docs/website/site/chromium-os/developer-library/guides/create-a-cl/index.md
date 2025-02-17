---
breadcrumbs:
- - /chromium-os/developer-library/guides/
  - ChromiumOS > Guides
page_name: create-a-cl
title: Create a CL
---

For a more thorough introduction, see the [appropriate section of the Developer
Guide](/chromium-os/developer-library/guides/development/developer-guide/#making-changes-to-packages-whose-source-code-is-checked-into-chromiumos-git-repositories).

Here's the basic flow when creating/uploading a CL for review:

1.  Navigate to the project you want to create a CL for, I'll use
            src/scripts as an example: `cd ~/trunk/src/scripts`
2.  Start a new branch that tracks the remote branch: `repo start
            <name of branch to create> .`
3.  Make the changes you want to upload, and then commit them, following
            the [commit message
            format](/chromium-os/developer-library/guides/development/contributing/#Commit-messages).
4.  Upload your CL by typing `repo upload . --cbr`
5.  You'll get given a URL to your CL on Gerrit where it'll be reviewed.
            Add some reviewers and wait for them to check your change.
6.  Once you've been given +2 for your change, mark it verified assuming
            you've verified it works and commit ready.
7.  Wait for the commit queue to pick your change up. If it passes the
            commit queue, it'll be merged. If it fails, a comment will be posted
            on the review and you'll get an email telling you as such.
8.  Congrats! You've contributed your change!
