/**
 * @file main.c
 *
 * @brief Main entry point for the dive computer capstone.
 */
#include "main.h"
#include "Board.h"
#include "scuba.h"

// Entry point
int main(void)
{
    /* Call driver init functions */
    Board_initGeneral();

    //! @todo initialize startup task parameters, stack size, priority

    //! @todo initialize error block

    //! @todo create the startup task

    //! @todo check for problems using SYS_ASSERT()

    //! @todo start multitasking

    return (0);
}
