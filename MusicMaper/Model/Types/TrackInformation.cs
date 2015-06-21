using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MusicMaper.Models
{
    public class TrackInformation
    {
        public string Artist { get; set; }
        public string Title { get; set; }
        public float Duration { get; set; }
    }
}
