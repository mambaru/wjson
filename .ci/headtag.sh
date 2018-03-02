#!/bin/bash
tag=$(git describe --abbrev=0 --tags 2>/dev/null)
echo "tag: $tag"
echo "branch: $1"
commits_head=$(git rev-list HEAD 2>/dev/null | wc -l 2>/dev/null)
echo "commits_head: $commits_head"
commits_tag=$(git rev-list $tag 2>/dev/null | wc -l 2>/dev/null)
echo "commits_tag: $commits_tag"
untaged_commits=`expr $commits_head - $commits_tag`
echo "untaged_commits: $untaged_commits"
exit $untaged_commits
