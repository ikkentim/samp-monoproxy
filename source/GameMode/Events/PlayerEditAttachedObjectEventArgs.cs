﻿using GameMode.Definitions;
using GameMode.World;

namespace GameMode.Events
{
    public class PlayerEditAttachedObjectEventArgs : PlayerClickMapEventArgs
    {
        public PlayerEditAttachedObjectEventArgs(int playerid, EditObjectResponse response, int index, int modelid,
            int boneid, Position position, Rotation rotation, Position offset) : base(playerid, position)
        {
            EidEditObjectResponse = response;
            Index = index;
            ModelId = modelid;
            BoneId = boneid;
            Rotation = rotation;
            Offset = offset;
        }

        public EditObjectResponse EidEditObjectResponse { get; private set; }

        public int Index { get; private set; }

        public int ModelId { get; private set; }

        public int BoneId { get; private set; }

        public Rotation Rotation { get; private set; }

        public Position Offset { get; private set; }

    }
}
