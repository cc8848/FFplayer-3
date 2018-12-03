/*  1. rename url
 *      can work, but it can't push error when this function didn't work.
 *  danger !!!
 */
int err_code = -1;
err_code = avpriv_io_move("../testData/Romantic321.ts", "../testData/Romantic.ts");
if(err_code < 0)
    FUNC_ERROR(err_code);


/*  2. delete url
 *      can't work, error: Invalid argument.
 */
err_code = avpriv_io_delete("../testData/111.txt");
if(err_code < 0)
    FUNC_ERROR(err_code);


/*  3. open dir + read dir + close dir
 *      can't work, error in "avio_open_dir()", error info: Function not implemented.
 */
AVIODirContext *ctx = NULL;
AVIODirEntry *entry = NULL;
int ret = avio_open_dir(&ctx, "../testData", NULL);
if(ret < 0)
    FUNC_ERROR(ret);     //av_err2str(ret);

while(1){
    ret = avio_read_dir(ctx, &entry);
    if(ret < 0)
        FUNC_ERROR(ret);
    if(!entry)
        break;
    qDebug() <<"entry->size = " <<entry->size
            <<"entry->name = " <<entry->name;

    avio_free_directory_entry(&entry);
}
avio_close_dir(&ctx);


