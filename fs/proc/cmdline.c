#include <linux/fs.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/setup.h>

static char updated_command_line[COMMAND_LINE_SIZE];

static int cmdline_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "%s\n", updated_command_line);
	return 0;
}

static int cmdline_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, cmdline_proc_show, NULL);
}

static const struct file_operations cmdline_proc_fops = {
	.open		= cmdline_proc_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int __init proc_cmdline_init(void)
{
	char *flag_pos, *flag_after;

	strcpy(updated_command_line, saved_command_line);
	
	flag_pos = strstr(updated_command_line, "androidboot.verifiedbootstate=");
	if (flag_pos) {
		flag_after = strchr(flag_pos, ' ');
		if (!flag_after)
			flag_after = "";

		scnprintf(updated_command_line, COMMAND_LINE_SIZE, "%.*sandroidboot.verifiedbootstate=green%s",
				(int)(flag_pos - updated_command_line + 1),
				updated_command_line, flag_after);
	} else {
		// flag was found, insert it
		scnprintf(updated_command_line, COMMAND_LINE_SIZE, "%s androidboot.verifiedbootstate=green", updated_command_line);
	}

	proc_create("cmdline", 0, NULL, &cmdline_proc_fops);
	return 0;
}
module_init(proc_cmdline_init);
