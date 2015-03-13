using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;
using Windows.Storage;
using Windows.Storage.AccessCache;

namespace MediaData
{
    public static class FolderHelper
    {
        private static List<StorageFolder> storages = null;
        private static StorageFolder currentFolder = null;

        public static IReadOnlyList<StorageFolder> Storages
        {
            get
            {
                return FolderHelper.storages;
            }
        }

        public static StorageFolder LatestFolder
        {
            get
            {
                return FolderHelper.currentFolder;
            }
        }

        public static void AddStorage(StorageFolder folder)
        {
            bool haveFolder = false;

            foreach (var i in FolderHelper.storages)
            {
                if (string.Compare(i.Path, folder.Path) == 0)
                {
                    haveFolder = true;
                    break;
                }
            }

            if (!haveFolder)
            {
                var count = FolderHelper.storages.Count;
                var name = "AudioFolder" + count;
                StorageApplicationPermissions.FutureAccessList.AddOrReplace(name, folder);
                FolderHelper.storages.Add(folder);
                ApplicationData.Current.LocalSettings.Values["AudioStoragesCount"] = FolderHelper.storages.Count;
            }

            FolderHelper.currentFolder = folder;
        }

        public static Task InitStorages()
        {
            return Task.Run(async () =>
            {
                if (FolderHelper.storages == null)
                {

                    FolderHelper.storages = new List<StorageFolder>();
                    int storageCount = 0;
                    var count = ApplicationData.Current.LocalSettings.Values["AudioStoragesCount"];

                    if (count != null)
                    {
                        storageCount = (int)count;
                    }

                    for (int i = 0; i < storageCount; i++)
                    {
                        try
                        {
                            var name = "AudioFolder" + i;
                            var folder = await StorageApplicationPermissions.FutureAccessList.GetFolderAsync(name);
                            FolderHelper.storages.Add(folder);
                        }
                        catch (Exception e)
                        {
                            var ex = e;
                        }
                    }

                    ApplicationData.Current.LocalSettings.Values["AudioStoragesCount"] = FolderHelper.storages.Count;
                    if (FolderHelper.storages.Count != 0)
                    {
                        FolderHelper.currentFolder = FolderHelper.storages[FolderHelper.storages.Count - 1];
                    }
                }
            });
        }

    }
}
