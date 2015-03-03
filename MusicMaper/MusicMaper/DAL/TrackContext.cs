using System;
using System.Collections.Generic;
using System.Data.Entity;
using System.Data.Entity.Design;
using System.Data.Entity.ModelConfiguration.Conventions;
using MusicMaper.Models;

namespace MusicMaper.DAL
{
    public class TrackContext : DbContext
    {
        public DbSet<PlayerNode> Players { get; set; }
        protected override void OnModelCreating(DbModelBuilder modelBuilder)
        {
            modelBuilder.Conventions.Remove<PluralizingTableNameConvention>();
        }  
    }
}