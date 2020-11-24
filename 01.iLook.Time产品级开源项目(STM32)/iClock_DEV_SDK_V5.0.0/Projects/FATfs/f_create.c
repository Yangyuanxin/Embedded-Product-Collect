FRESULT f_create (
	FIL *fp,			/* Pointer to the blank file object */
	const char *path,	/* Pointer to the file name */
	DWORD ofs		/* File pointer from top of file */
)
{
	FRESULT res;
	DWORD clst, bcs, ps;
    BYTE mode;
    BYTE *dir;
    DIR dj;
	NAMEBUF(sfn, lfn);

	fp->fs = NULL;		/* Clear file object */
	mode = FA_READ | FA_WRITE | FA_CREATE_ALWAYS | FA_OPEN_ALWAYS | FA_CREATE_NEW;
	res = auto_mount(&path, &dj.fs, (BYTE)(mode & (FA_WRITE | FA_CREATE_ALWAYS | FA_OPEN_ALWAYS | FA_CREATE_NEW)));
//create file
	if (res != FR_OK) LEAVE_FF(dj.fs, res);
	INITBUF(dj, sfn, lfn);
	res = follow_path(&dj, path);	/* Follow the file path */

	if (res != FR_OK) {		/* No file, create new */
		if (res == FR_NO_FILE)
			res = dir_register(&dj);
		if (res != FR_OK) LEAVE_FF(dj.fs, res);
		mode |= FA_CREATE_ALWAYS;
		dir = dj.dir;
	}
	else {					    /* Any object is already existing */
        LEAVE_FF(dj.fs, FR_EXIST);
	}
    if (mode & FA_CREATE_ALWAYS) {
	dir[DIR_Attr] = 0;					/* Reset attribute */
	ps = get_fattime();
	ST_DWORD(dir+DIR_CrtTime, ps);		/* Created time */
	dj.fs->wflag = 1;
	mode |= FA__WRITTEN;				/* Set file changed flag */
	}
    fp->dir_sect = dj.fs->winsect;		/* Pointer to the directory entry */
	fp->dir_ptr = dj.dir;
	fp->flag = mode;					/* File access mode */
	fp->org_clust =						/* File start cluster */
		((DWORD)LD_WORD(dir+DIR_FstClusHI) << 16) | LD_WORD(dir+DIR_FstClusLO);
	fp->fsize = LD_DWORD(dir+DIR_FileSize);	/* File size */
	fp->fptr = 0; fp->csect = 255;		/* File pointer */
	fp->dsect = 0;
	fp->fs = dj.fs; fp->id = dj.fs->id;	/* Owner file system object of the file */
	res = f_sync(fp);	
//·ÖÅä´Ø¿Õ¼ä
    //create a new chain
    clst = create_chain(fp->fs, 0);
    if (clst == 1) 
        ABORT(fp->fs, FR_INT_ERR);
    if (clst == 0xFFFFFFFF) 
        ABORT(fp->fs, FR_DISK_ERR);
    fp->org_clust = clst;
//flseek
    bcs = (DWORD)fp->fs->csize * SS(fp->fs);	/* Cluster size (byte) */
    while (ofs > bcs) {						/* Cluster following loop */
		clst = create_chain(fp->fs, clst);	/* Force streached if in write mode */
		if (clst == 0) {				/* When disk gets full, clip file size */
			ofs = bcs; break;
		}
		if (clst == 0xFFFFFFFF) 
		    ABORT(fp->fs, FR_DISK_ERR);
		if (clst <= 1 || clst >= fp->fs->max_clust) 
		    ABORT(fp->fs, FR_INT_ERR);
		fp->curr_clust = clst;
		fp->fptr += bcs;
		ofs -= bcs;
    }
	fp->fptr += ofs;
	fp->csect = (BYTE)(ofs / SS(fp->fs));	/* Sector offset in the cluster */
	fp->fsize = fp->fptr;
	fp->flag |= FA__WRITTEN;
//
    res = f_sync(fp);
	if (res == FR_OK) fp->fs = NULL;
	
    LEAVE_FF(dj.fs, res);
}


