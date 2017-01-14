int main(PREQPACKET rp, int dev)
{
    switch(rp->RPcommand)
    {
    case RPINIT:          /* 0x00                      */

        /* init called by kernel in protected mode */

        return Init(rp);

    case RPREAD:          /* 0x04                      */

        return (RPDONE);

    case RPWRITE:         /* 0x08                      */

        return (RPDONE);         

    case RPINPUT_FLUSH:   /* 0x07                      */

        return (RPDONE);

    case RPOUTPUT_FLUSH:  /* 0x0b                      */

        return (RPDONE);

    case RPOPEN:          /* 0x0d                      */

        return (RPDONE);

    case RPCLOSE:         /* 0x0e                      */

         return (RPDONE);
    case RPIOCTL:         /* 0x10                      */

        switch (rp->s.IOCtl.function)
        {
        case 0x00:        /* our function def #1       */

            return (RPDONE);

        case 0x01:        /* our function def #2       */

            return (RPDONE);
        }

    /* deinstall request */

    case RPDEINSTALL:     /* 0x14                      */

        return(RPDONE | RPERR | ERROR_BAD_COMMAND);

    /* all other commands are flagged */

    default:
        return(RPDONE | RPERR | ERROR_BAD_COMMAND);

    }
}


