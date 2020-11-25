#!/usr/bin/perl
use Cwd;
use File::Copy;

my $dir = getcwd;

my $mark = "VERSION_NUMBER";

my $mark_oem_code = "UPDATE_OEM_CODE";

my $mark_dev_id = "UPDATE_DEVICE_CODE";

my $version;

my $checksum;

my $getverflag = 0;

my $cfile = $dir."/inc/config_service.h";

my $ofile = $dir."/output/GmAppMain.ex";

my $bfile = $dir."/output/GmAppMain.bin";

my $release = $dir."/release";

my $mfile;

sub set_zero_to_file{

	my $t_handle = $_[0];
	
	my $t_len = $_[1];
	
	my $zero = "00";
	
	my $set_buf = pack("H*",$zero);
	
	if($t_len > 0)
	{
		for(my $cnt = 0; $cnt < $t_len ; $cnt++)
		{
			syswrite($t_handle,$set_buf,1);
		}
	}
	
	
}

#my $dir = getcwd;

my $build_ofile = $dir."/build/GmAppMain.ex";

my $build_bfile = $dir."/build/GmAppMain.bin";

my $out_ofile = $dir."/output/GmAppMain.ex";

my $out_bfile = $dir."/output/GmAppMain.bin";

my $update_file = $dir."/output/GMAppMain.m";

my $output_dir = $dir."/output";

my $oem_ofset = 0x31A69;

#my @upd_oem = (0,0,0,0,0,0);
my $upd_oem;

my $devid_fset = 0x31A7A;

#my @upd_devid = (0,0,0,0,0);
my $upd_devid;

my @upd_checksum = (0,0,0,0,0,0,0,0,0,0);

#my @upd_bootchecksum = (0x46,0x46,0x46,0x46,0x46,0x46,0x46,0x46,0,0);
my $upd_bootchecksum = "FFFFFFFF";

my $ver_ofset = 0x315B7;

#my @upd_version = (0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
my $upd_version;

my $checksum = 0;

if(-e $output_dir)
{

}
else
{
	mkdir $output_dir;
}

if(-e $cfile){
	printf "$cfile is exit!!\r\n";
}else{
	exit;
}

open(VERNO,"<".$cfile) or die $cfile."无法打开",$!;

@lines = <VERNO>;

my @l2_split;

foreach $line (@lines)
{
	@l_split = split(" ",$line);
	
	if($l_split[1] eq $mark)
	{
		@l2_split = split("\"",$l_split[2]);
		
		printf "\r\n\version $l2_split[1]\r\n";
			
		$version = $l2_split[1];
		
		$upd_version = $l2_split[1];
			
		$getverflg = 1;

	}
	
    if($l_split[1] eq $mark_oem_code)
	{
		@l2_split = split("\"",$l_split[2]);
		
		printf "OEM ".$l2_split[1];
		
		$upd_oem = $l2_split[1];
	}
	
    if($l_split[1] eq $mark_dev_id)
	{
		@l2_split = split("\"",$l_split[2]);
		
		$upd_devid = substr($l2_split[1],8);
		
		printf "\r\n\dev id $upd_devid\r\n";
			
		last;
	}

	
}

close(VERNO) || die "无法关闭文件";


copy($build_bfile,$out_bfile)||warn "could not copy file : $!";

copy($build_ofile,$out_ofile)||warn "could not copy file : $!";

if(!-e $out_bfile)
{
	printf "$out_bfile not exit!!";
	die;
}

if(!-e $out_ofile)
{
	printf "$out_ofile not exit!!";
	die;
}


open (BINHANDLE,$out_bfile) || die "无目标文件,$!";

binmode(BINHANDLE);

$checksum = 0;

my $read_len = 1;

my $index = 0;

my $bin_hex = 0;

my $bin = 0;

my $buf = 0;

my $arry_cnt = 0;


$index = 0;

while($read_len > 0)
{
	
	$read_len = sysread(BINHANDLE,$buf,1);
	
	if($read_len >0)
	{
		$bin_hex = unpack("H*",$buf);
		
		$bin = hex($bin_hex);
		
		#$file_byte = sprintf("%x",$bin^0x0F);
		
		#syswrite(UPDATEHAN,pack("h*",$file_byte),1);
		
		$checksum = $checksum + $bin;
		
		$index++;
		
	}
	
	
}

my $checksum_hex = uc(sprintf("%x",$checksum));


close BINHANDLE;

#close UPDATEHAN;

open (UPDATEHAN,">$update_file");

open (BINHANDLE,$out_bfile) || die "无目标文件,$!";

binmode(BINHANDLE);

binmode(UPDATEHAN);

syswrite(UPDATEHAN,$upd_oem,length($upd_oem));

set_zero_to_file(UPDATEHAN,6-length($upd_oem));

syswrite(UPDATEHAN,$upd_devid,length($upd_devid));#6-length($upd_oem)

set_zero_to_file(UPDATEHAN,5-length($upd_devid));

syswrite(UPDATEHAN,$upd_version,length($upd_version));

set_zero_to_file(UPDATEHAN,20-length($upd_version));

syswrite(UPDATEHAN,$checksum_hex,length($checksum_hex));

set_zero_to_file(UPDATEHAN,10-length($checksum_hex));

syswrite(UPDATEHAN,$upd_bootchecksum,length($upd_bootchecksum));

set_zero_to_file(UPDATEHAN,10-length($upd_bootchecksum));

for(my $len_cnt = 4 ; $len_cnt > 0 ; $len_cnt--)
{
	my $len_byte = ($index >> (($len_cnt - 1)*8))&0xFF;
	
	my $len_string = sprintf("%x",$len_byte);
	
	if($len_cnt == 1)
	{
		syswrite(UPDATEHAN,pack("H*",$len_string),1);
	}
	else
	{
		syswrite(UPDATEHAN,pack("h*",$len_string),1);
	}
	
}

set_zero_to_file(UPDATEHAN,73);

$read_len = 1;

while($read_len > 0)
{
	
	$read_len = sysread(BINHANDLE,$buf,1);
	
	if($read_len >0)
	{
		$bin_hex = unpack("H*",$buf);
		
		$bin = hex($bin_hex);
		
		$file_byte = sprintf("%x",$bin^0x0F);
		
		if($bin < 0x10)
		{
			syswrite(UPDATEHAN,pack("h*",$file_byte),1);
		}
		else
		{
			syswrite(UPDATEHAN,pack("H*",$file_byte),1);
		}
		
		
		#$checksum = $checksum + $bin;
		
		#$index++;
		
	}
	
	
}

close UPDATEHAN;

close BINHANDLE;

my $new_name = "(".$checksum_hex.")";

rename($update_file,$dir."/output/".$new_name."GMAppMain.m");

#rename($out_ofile,$dir."/output/".$new_name."GMAppMain.ex");

#rename($out_bfile,$dir."/output/".$new_name."GMAppMain.bin");

my @allfiles=glob($dir."/output/*");


if($getverflg == 0)
{
	exit;
}

foreach $file (@allfiles){
     
	 #printf $file."\r\n";
	 
	 @l1_file = split(/\./,$file);
	 
	 #printf $l1_file[1]."\r\n";
	 
	 if($l1_file[1] eq "m")
	 {
		@l2_file = split(/\(/,$l1_file[0]);

		#printf $l2_file[1]."\r\n";
		
		if($l2_file[1])
		{
			@l3_file = split(/\)/,$l2_file[1]);
			
			#printf $l3_file[0]."\r\n";
			
			$checksum = "(".$l3_file[0].")";
			
			$mfile = $file;
			
			last;
		}
	 }
}

($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime();
$year = $year + 1900;
$mon = $mon + 1;
#$now = $year.$mon.$mday.$hour.$min.$sec;
$now = sprintf("%d%02d%02d%02d%02d%02d",$year,$mon,$mday,$hour,$min,$sec);
if(-e $release){

}else{
	mkdir $release
}

$newversom_dir = $release."/".$version."_".$now;

mkdir $newversom_dir;

if(-e $ofile){
	#copy $ofile,$dir."/output/".$checksum.$version.".ex";
	copy $ofile,$newversom_dir."/".$checksum.$version.".ex";
}else{
	printf "ofile not exit!\r\n";
}

if(-e $mfile){
	#rename $mfile,$dir."/output/".$checksum.$version.".m";
	copy $mfile,$newversom_dir."/".$checksum.$version.".m";
}else{
	printf "mfile not exit!\r\n";
}

if(-e $bfile){
	copy $bfile,$newversom_dir."/".$checksum.$version.".bin";
}else{
	print "bfile not exit!\r\n"
}


