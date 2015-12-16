// SampSharp
// Copyright 2015 Tim Potze
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

using System.Globalization;
using System.Linq;

namespace SampSharp.GameMode.SAMP.Commands.Arguments
{
    public class IntegerCommandParameterType : ICommandParameterType
    {
        private static readonly char[] Base10Characters = "1234567890,.".ToCharArray();
        private static readonly char[] Base16Characters = "1234567890abcdef".ToCharArray();

        #region Implementation of ICommandParameterType

        public bool GetValue(ref string commandText, out object output)
        {
            var text = commandText.TrimStart();

            if (string.IsNullOrEmpty(text))
            {
                output = null;
                return false;
            }

            var word = text.Split(' ').First();

            int number;

            // Regular base 10 numbers (eg. 14143)
            if (word.All(Base10Characters.Contains) && int.TryParse(word, out number))
            {
                commandText = word.Length == commandText.Length
                    ? string.Empty
                    : commandText.Substring(word.Length).TrimStart(' ');

                output = number;
                return true;
            }

            // Base 16 (hexidecimal) numbers. Can be prefixed with '0x', '#' or postfixed with 'H' or 'h'.
            string base16Word = null;
            if (word.Length > 2 && word.StartsWith("0x"))
                base16Word = word.Substring(2);
            else if (word.Length > 1 && word.StartsWith("#"))
                base16Word = word.Substring(1);
            else if (word.Length > 1 && word.ToLower().EndsWith("h"))
                base16Word = word.Substring(0, word.Length - 1);

            if (base16Word != null && base16Word.ToLower().All(Base16Characters.Contains) &&
                int.TryParse(base16Word, NumberStyles.AllowHexSpecifier, CultureInfo.InvariantCulture, out number))
            {
                commandText = word.Length == commandText.Length
                    ? string.Empty
                    : commandText.Substring(word.Length).TrimStart(' ');

                output = number;
                return true;
            }

            output = null;
            return false;
        }

        #endregion
    }
}