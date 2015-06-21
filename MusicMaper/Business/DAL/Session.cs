using MusicMaper.Models;
using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Business.DAL
{
    public class Session
    {
        [Key]
        public int Id { get; set; }
        public DeviceInformation Device { get; set; }
        public TrackInformation Track { get; set; }
        public DateTime Connected { get; set; }
    }
}
