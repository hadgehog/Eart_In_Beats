using MusicMaper.Models;
using System;
using System.Collections.Generic;
using System.Data.Entity;
using System.Data.Entity.ModelConfiguration.Conventions;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Business.DAL
{
    public class DataBaseContext : DbContext
    {
        public DbSet<Session> Sessions { get; set; }

        public DataBaseContext()
            : base("DefaultConnection")
        {

        }

        protected override void OnModelCreating(DbModelBuilder modelBuilder)
        {
            modelBuilder.Conventions.Remove<PluralizingTableNameConvention>();
        }

    }

    public class DataBaseInitiaizator : DropCreateDatabaseIfModelChanges<DataBaseContext>
    {
        protected override void Seed(DataBaseContext context)
        {
            

            for (int i = 0; i < 10; i++)
            {
                
            }
            context.SaveChanges();

            for (int i = 0; i < 20; i++)
            {
               
            }

            context.SaveChanges();
        }

    }
}



