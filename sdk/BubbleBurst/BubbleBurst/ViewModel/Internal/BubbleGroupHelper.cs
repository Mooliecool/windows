using ReactiveUI;
using System;
using System.Collections.Generic;
using System.Linq;

namespace BubbleBurst.ViewModel.Internal
{
    /// <summary>
    /// Locates and stores a set contiguous bubbles of the same color.
    /// Exposes methods to activate and deactivate a group of bubbles,
    /// which is used to highlight them in the UI.
    /// </summary>
    internal class BubbleGroupHelper : ReactiveObject
    {
        readonly IEnumerable<BubbleViewModel> _allBubbles;

        List<BubbleViewModel> _bubblesInGroup = new List<BubbleViewModel>();

        bool HasBubbles { get { return this._bubblesInGroup.Any(); } }

        internal BubbleGroupHelper(IEnumerable<BubbleViewModel> allBubbles)
        {
            if (allBubbles == null)
                throw new ArgumentNullException("allBubbles");

            _allBubbles = allBubbles;
        }

        internal void Activate()
        {
            foreach (BubbleViewModel member in _bubblesInGroup)
            {
                member.IsActive = true;
            }
        }

        internal void Deactivate()
        {
            foreach (BubbleViewModel member in _bubblesInGroup)
            {
                member.IsActive = false;
            }
        }

        internal bool AnyGroupsExist()
        {
            return _allBubbles.Any(b => FindBubbleGroup(b).HasBubbles);
        }

        /// <summary>
        /// Searches for a bubble group in which the specified bubble
        /// is a member.  If a group is found, this object's BubblesInGroup
        /// collection will contain the bubbles in that group afterwards.
        /// </summary>
        internal BubbleGroupHelper FindBubbleGroup(BubbleViewModel bubble)
        {
            if (bubble == null)
                throw new ArgumentNullException("bubble");

            bool isBubbleInCurrentGroup = this._bubblesInGroup.Contains(bubble);
            if (!isBubbleInCurrentGroup)
            {
                this._bubblesInGroup.Clear();

                this.SearchForGroup(bubble);

                bool addOriginalBubble =
                    this.HasBubbles &&
                    !this._bubblesInGroup.Contains(bubble);

                if (addOriginalBubble)
                {
                    this._bubblesInGroup.Add(bubble);
                }
            }
            return this;
        }

        internal void Reset()
        {
            this.Deactivate();
            _bubblesInGroup.Clear();
        }

        void SearchForGroup(BubbleViewModel bubble)
        {
            if (bubble == null)
                throw new ArgumentNullException("bubble");

            foreach (BubbleViewModel groupMember in this.FindMatchingNeighbors(bubble))
            {
                if (!_bubblesInGroup.Contains(groupMember))
                {
                    _bubblesInGroup.Add(groupMember);
                    this.SearchForGroup(groupMember);
                }
            }
        }

        IEnumerable<BubbleViewModel> FindMatchingNeighbors(BubbleViewModel bubble)
        {
            var matches = new List<BubbleViewModel>();

            // Check above.
            var match = this.TryFindMatch(bubble.Row - 1, bubble.Column, bubble.BubbleType);
            if (match != null)
                matches.Add(match);

            // Check below.
            match = this.TryFindMatch(bubble.Row + 1, bubble.Column, bubble.BubbleType);
            if (match != null)
                matches.Add(match);

            // Check left.
            match = this.TryFindMatch(bubble.Row, bubble.Column - 1, bubble.BubbleType);
            if (match != null)
                matches.Add(match);

            // Check right.
            match = this.TryFindMatch(bubble.Row, bubble.Column + 1, bubble.BubbleType);
            if (match != null)
                matches.Add(match);

            return matches;
        }

        BubbleViewModel TryFindMatch(int row, int column, BubbleType bubbleType)
        {
            return _allBubbles.SingleOrDefault(b =>
                b.Row == row &&
                b.Column == column &&
                b.BubbleType == bubbleType);
        }
    }
}