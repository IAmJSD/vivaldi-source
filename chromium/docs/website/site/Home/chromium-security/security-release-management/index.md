---
breadcrumbs:
- - /Home
  - Chromium
- - /Home/chromium-security
  - Chromium Security
page_name: security-release-management
title: Security Release Management
---

[TOC]

## **Merging Fixes**

Engineers outside the Chrome Security team shouldn’t request merges for security
fixes. Instead, please [mark the bug as
Fixed](https://chromium.googlesource.com/chromium/src/+/refs/heads/main/docs/security/security-labels.md#TOC-Merge-labels)
and we’ll take care of it. The instructions below are for the Chrome Security
team itself.

### **TODO(chrome-security@): Identifying fixes to merge.**

Security fixes often need to be merged to the Stable or Beta branch. One of the
key considerations in picking a fix to merge is assessing the risk of breakage,
which the bug’s owner should be able to convey. For risky fixes you should
consider bumping it to a later milestone. Generally, you should consider merging
the following:

*   [Critical (S0) or High (S1) severity bugs](/developers/severity-guidelines)
*   [Medium (S2) severity bugs](/developers/severity-guidelines) if they're
            particularly worrisome (e.g. info leak) or have significant
            non-security impact (e.g. top crasher).
*   [Low (S3) severity bugs](/developers/severity-guidelines) where the fixes
            are very low risk.

For Critical (S0) and High (S1) severity bugs, we aim to fix users fast and work directly
with Chrome TPMs to make sure any merge or release is done safely. The following are
general recommendations on when / where to merge:

*   ==Released Critical vulnerability==: Emergency, out of band release,
            target a fix as soon as we have a viable release candidate.
*   ==Unreleased Critical vulnerability==: Target a fix for the next
            scheduled Stable point release. (An exception here is if we know it
            will be released prior to the next scheduled release, then we push
            as fast as needed to address it).
*   ==Released High vulnerability==: A fix should be merged to the next
            scheduled Stable point release.
*   ==Unreleased High (and lower severity) vulnerability==: Target the
            merge to the next scheduled Stable milestone release.

Other factors to consider in your merge vs. no-merge calculation are:

*   **Bake time**. If a fix has been landed on trunk for weeks and made
            it into various dev channels with no observed screaming, a merge
            candidate is said to be "well baked". A well baked patch might be
            considered for merge even if it is of lower severity, or appears
            scary. Conversely, we should rarely be merging anything that hasn't
            landed for a few days and been released to a dev channel.
*   **Ease of discovery**. If a bug has been found independently by
            multiple efforts (e.g. an external researcher as well as ClusterFuzz
            internally), this suggests that the bug is readily discoverable and
            should be merged sooner rather than later.
*   **Ease of merge**. If the merge is a total nightmare, it may be fair
            to not merge the fix and just wait for the fix on trunk to "roll
            into stable", as all work on trunk eventually does. Of course, if
            the bug is critical or other factors (ease of discovery, highly
            exploitable, etc.) apply, then we should bite the bullet and
            undertake the extra work in order to best protect users.
*   **Change of behavior**. If the fix in fact changes behavior (locks
            something down, turns something off, etc.) then it is best to have
            the fix appear as a major revision bump as opposed to a patch on the
            stable channel. This doesn't mean "don't merge" but it does suggest
            we might merge to beta but not the existing stable.

Your worklist for merges is defined by the following bug database criteria:

*   All Issues
*   Type=Vulnerability
*   customfield1223134:&lt;Milestone&gt; (Merge-Request) or
customfield1223087:Review-&lt;Milestone&gt;

(Note: beware of querying on the milestone. At this stage in the bug's life, the
milestone label represents the earliest affected release at the time the bug was
filed. So if current stable is M122, there might still be bugs in this list which
are M121, M120, etc., if we've been a little slow in fixing them. Conversely,
anything marked M122 denotes a regression since M121 stable, so it only needs to
be merged to the M122 beta.)

### **TODO(bug owner): Merging a security patch.**

When a security merge has been approved, please go ahead and merge.

To avoid accidentally losing a fix for a Chrome release, branch merges should be
done in a "newer first" manner. For example, if you're merging a fix to M121
stable and M122 is branched and affected, then merge to M122 first. Such a
strategy can also be used to bake a fix on the M122 branch in order to gain
confidence about an M121 stable channel merge.

#### **How to merge**

You should merge fixes with [gerrit](/developers/how-tos/drover), which
automates most of the process. If the [gerrit merge](/developers/how-tos/drover)
does not apply cleanly you may want to reconsider whether the fix will introduce
an unacceptable breakage risk. In the event that you can’t use gerrit to merge a
fix, you still have the option of manually merging from a branch checkout. This
follows the normal developer
[normal patch process](https://chromium.googlesource.com/chromium/src/+/main/docs/contributing.md).

#### **Post-merge bug cleanup.**

Once the fix is successfully merged you will need to:

1.  Update the bug with the merge revision numbers (one for each branch
            you've merged to).
2.  Set the merge status to Merge-Approved to Merge-Merged (if drover
            didn't do so).

**Release Notes**

For every new release, we include notes from security bugs that match the
following search criteria:

*   Type=Vulnerability
*   M=? (Milestone, customfield1223085:yyy)
*   Security_Release=x-Myyy (customfield1223088:x-Myyy)
*   -Severity=S4
*   Security_Impact-Stable (hotlistid:5432902)

For example, [this
link](https://issues.chromium.org/issues?q=Type%3DVulnerability%20customfield1223085:122%20Release%3D0-M122%20-Severity:S4%20--customfield1223084:Android)
shows the security fixes that went into the initial Beta -&gt; Stable promotion
of Chrome 122, and [this
link](https://issues.chromium.org/issues?q=Type%3DVulnerability%20customfield1223085:122%20Release%3D1-M122%20-Severity:S4%20--customfield1223084:Android)
shows the security fixes that went into the first post-stable patch of the
Chrome 122 branch.

Every externally reported issue gets assigned a CVE ID per MITRE's [CVE Counting
Rules](https://cve.mitre.org/cve/editorial_policies/counting_rules.html).
Chrome's CVE pool is
[here](https://docs.google.com/a/google.com/spreadsheet/ccc?key=0AoQyc9BFHd9FdE43bkxKZFRtOEk3eW5zeEhST29zTmc#gid=0)
(Google internal-only link, sorry). As of Chrome 27, we're focusing the release
notes on externally reported issues. This mirrors how Mozilla Firefox arranges
their release notes, saves the precious resource of CVEs, saves a lot of time in
preparing release notes, and appropriately focuses our security release notes on
the excellent contributions and rewards of external researchers.

CVEs are allocated when the stable release that contains the fix is released,
and they are then placed into the bug tracker using the CVE label, and copied
into the release notes. An example of how it looks is [here for the Chrome 121
release
notes](https://chromereleases.googleblog.com/2024/01/stable-channel-update-for-desktop_23.html).

### **security-notify@chromium.org**

There is no longer any need to pre-notify security-notify@chromium.org with a
copy of release notes. Very few list members were finding it useful or
actionable.
